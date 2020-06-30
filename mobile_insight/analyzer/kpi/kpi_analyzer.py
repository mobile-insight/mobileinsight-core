#!/usr/bin/python
# Filename: kpi_analyzer.py
"""
kpi_analyzer.py
An abstraction for KPI analyzer

Author: Yuanjie Li
"""

is_android = False
try:
    from jnius import autoclass  # For Android
    try:
        from service import mi2app_utils 
        PythonService = autoclass('org.kivy.android.PythonService')
        pyService = PythonService.mService    
        Context = autoclass('android.content.Context')
        ConnectivityManager = pyService.getSystemService(Context.CONNECTIVITY_SERVICE)   
    except Exception as e:
        import main_utils
    is_android = True
except Exception as e:
    import sqlite3  # Laptop version
    is_android = False

__all__ = ["KpiAnalyzer"]

from ..analyzer import *
# from .track_cell_info_analyzer import TrackCellInfoAnalyzer
import os, errno
import urllib.request, urllib.error, urllib.parse, json, time, datetime
import threading
from collections import deque


class KpiAnalyzer(Analyzer):

    """
    An abstraction for KPI analyzer. It offers three functions

        (1) Helpers to simplify the development of KPI tracking

        (2) Automation of the KPI local storage management

        (3) Automation of the KPI uploading to KPI map (cloud). 
    """

    # Global variables: For asynchrounous KPI upload
    upload_thread = None
    pending_upload_task = deque([]) # (kpi_name, kpi_val) pair list


    def __init__(self):

        Analyzer.__init__(self)
        self.include_analyzer('TrackCellInfoAnalyzer', [])

        # initilize local database
        self.supported_kpis = {} # Supported KPIs: kpi_name -> callback
        self.__db = None # Local dabatase: kpi_name -> database
        self.__conn = None # Local database cursor: kpi_name -> database
        self.__op = ""
        self.__phone_model = ""
        self.__db_enabled = False
        self.__periodicity = {}
        self.__logcell = {}
        self.__last_updated = {}

        # Initialize uploading thread
        if is_android and not KpiAnalyzer.upload_thread:
            e = threading.Event()
            KpiAnalyzer.upload_thread = threading.Thread(target=self.__upload_kpi_thread, args=(e,))
            KpiAnalyzer.upload_thread.start()

    def __del__(self):
        if is_android:
            mi2app_utils.detach_thread()

    def enable_local_storage(self, enable_storage):
        """
        Set if the local KPI should be stored

        :param enable_storage: Whether to locally store the kpi. False by default
        :type enable_storage: boolean
        """
        self.__db_enabled = enable_storage

    def register_kpi(self, kpi_type, kpi_name, callback, attributes = None):
        """
        Declare a KPI to be supported

        :param kpi_type: The type of the KPI (accessibility, retainability, mobility, availability, ...)
        :type kpi_type: string
        :param kpi_name: The name of the KPI
        :type kpi_name: string
        :param callback: The callbacks to update the KPI
        :type kpi_name: Python method
        :returns: True if the registeration succeeds, False otherwise (e.g., KPI already exists)
        :param attributes:
        :type attributes: None or a list of attributes
        """
        full_name = 'KPI.'+kpi_type+'.'+kpi_name
        if full_name in self.supported_kpis:
            # KPI already exists
            return False

        self.supported_kpis[full_name] = callback
        if not (self.__db and self.__conn):
            if not self.__create_db():
                self.log_info("Create database failed")
                return False
        if not self.__create_table(full_name, attributes):
            return False
        return True


    def __create_table(self, kpi_name, attributes):
        '''
        Create SQL tables for the kpi

        :param kpi_name: name of the kpi
        :type kpi_name: string
        :param attributes:
        :type attributes: None or a list of attributes
        '''
        kpi_name = kpi_name.replace('.', '_')
        if attributes:
            sql_cmd = 'CREATE TABLE IF NOT EXISTS ' + \
                      kpi_name + "(id integer primary key autoincrement, "
            for attribute in attributes:
                sql_cmd += (str(attribute) + ' text, ')
            sql_cmd += "timestamp timestamp, op text, phone_model text," \
                      "gps text, cell_id text, tai_id text, dl_freq text, ul_freq text, dl_bw text, ul_bw text," \
                      "allowed_access text, band_id text)"
        else:
            sql_cmd = 'CREATE TABLE IF NOT EXISTS ' + \
                      kpi_name + "(id integer primary key autoincrement, value text, timestamp timestamp, op text, phone_model text," \
                      "gps text, cell_id text, tai_id text, dl_freq text, ul_freq text, dl_bw text, ul_bw text," \
                      "allowed_access text, band_id text)"
        # print sql_cmd

        # for rrc_sr, it may have several types, shall we build a table for each types?
        if is_android:
            self.__db.execSQL(sql_cmd)
        else:
            self.__db.execute(sql_cmd)
            self.__conn.commit()


    def __create_db(self):

        """
        Create a local database for the KPI.
        The database is stored at /sdcard/mobileinsight/kpi/

        :returns: True if the database is successfully created (or already exists), False otherwise
        """
        db_name = "Kpi"
        try:
            if is_android:
                Environment = autoclass("android.os.Environment")
                state = Environment.getExternalStorageState()
                if not Environment.MEDIA_MOUNTED == state:
                    self.__db = None
                    return

                sdcard_path = Environment.getExternalStorageDirectory().toString()
                DB_PATH = os.path.join(sdcard_path, "mobileinsight/dbs")
                activity = autoclass('org.kivy.android.PythonActivity')
                if activity.mActivity:
                    self.__db = activity.mActivity.openOrCreateDatabase(
                        os.path.join(DB_PATH, db_name + '.db'), 0, None)
                else:
                    service = autoclass('org.kivy.android.PythonService')
                    self.__db = service.mService.openOrCreateDatabase(
                        os.path.join(DB_PATH, db_name + '.db'), 0, None)
            else:
                try:
                    os.makedirs('./dbs/')
                except OSError as exception:
                    if exception.errno != errno.EEXIST:
                        raise
                self.__conn = sqlite3.connect('./dbs/' + db_name + '.db')
                self.__db = self.__conn.cursor()
            return True
        except BaseException:  # TODO: raise warnings
            return False


    def list_kpis(self):
        """
        Return a list of available KPIs 

        :returns: a list of string, each of which is a KPI name
        """
        return list(self.supported_kpis.keys())

    def __db_query(self, sql_cmd):
        """
        Return query result of a sql_cmd
        """

        try:
            if is_android:
                sql_res = self.__db.rawQuery(sql_cmd, None)
            else:
                sql_res = self.__db.execute(sql_cmd).fetchall()

            # print sql_res
            # if sql_res.getCount()==0: #the id does not exist
            if (is_android and sql_res.getCount() == 0) or (
                        not is_android and len(sql_res) == 0):
                return None

            if is_android:
                sql_res.moveToFirst()
                # convert string to dictionary
                res = sql_res.getString(0)
            else:
                res = sql_res[0][0]

            return res
        except BaseException:  # TODO: raise warnings
            return None


    def local_query_kpi(self, kpi_name, cell_id = None, timestamp = None):
        """
        Query the phone's locally observed KPI

        :param kpi_name: The KPI to be queried
        :type kpi_name: string
        :param cell_id: cell global id
        :type cell_id: string
        :param timestamp: The timestamp of the KPI. If None, this function returns the latest KPI
        :type timestamp: datetime
        :returns: The KPI value, or None if the KPI is not available
        """
        if not self.__db_enabled:
            self.log_warning("Database is not enabled.")
            return None

        # cell_id = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_id()
        # cell_id = cell_id if cell_id else None

        kpi_name = kpi_name.replace('.', '_')
        # print kpi_name

        if kpi_name.endswith('SR'):

            if cell_id:
                if 'HO' in kpi_name:
                    kpi_suc = kpi_name[:-2]+'FAILURE'
                else:
                    kpi_suc = kpi_name[:-2]+'SUC'

                if timestamp:
                    sql_cmd = "select count(*) from " + kpi_suc + " where timestamp<\"" + \
                    str(timestamp) + "\" and cell_id=\"" + str(cell_id) +"\""
                else:
                    sql_cmd = "select count(*) from " + kpi_suc + " where cell_id=\"" + str(cell_id) +"\""
                # print sql_cmd
                suc_num = self.__db_query(sql_cmd)

                if 'HO' in kpi_name:
                    kpi_req = kpi_name[:-2]+'TOTAL'
                else:
                    kpi_req = kpi_name[:-2]+'REQ'

                if timestamp:
                    sql_cmd = "select count(*) from " + kpi_req + " where timestamp<\"" + \
                    str(timestamp) + "\" and cell_id=\"" + str(cell_id) +"\""
                else:
                    sql_cmd = "select count(*) from " + kpi_req + " where cell_id=\"" + str(cell_id) +"\""
                # print sql_cmd
                req_num = self.__db_query(sql_cmd)

            else:
                if 'HO' in kpi_name:
                    kpi_suc = kpi_name[:-2]+'FAILURE'
                else:
                    kpi_suc = kpi_name[:-2]+'SUC'

                if timestamp:
                    sql_cmd = "select count(*) from " + kpi_suc + " where timestamp<\"" + \
                    str(timestamp) + "\""
                else:
                    sql_cmd = "select count(*) from " + kpi_suc 
                # print sql_cmd
                suc_num = self.__db_query(sql_cmd)

                if 'HO' in kpi_name:
                    kpi_req = kpi_name[:-2]+'TOTAL'
                else:
                    kpi_req = kpi_name[:-2]+'REQ'

                if timestamp:
                    sql_cmd = "select count(*) from " + kpi_req + " where timestamp<\"" + \
                    str(timestamp) + "\""
                else:
                    sql_cmd = "select count(*) from " + kpi_req 
                # print sql_cmd
                req_num = self.__db_query(sql_cmd)

            # print suc_num, req_num

            if req_num and suc_num and int(req_num) > 0:
                if 'HO' in kpi_name:
                    return '{:.2f}'.format(float(req_num - suc_num)/int(req_num)*100)+'%'
                else:
                    return '{:.2f}'.format(float(suc_num)/int(req_num)*100)+'%'

            return None 

        elif kpi_name.endswith('SUC') or kpi_name.endswith('REQ') or \
         kpi_name.endswith('TOTAL') or kpi_name.endswith('FAILURE'):
            if cell_id:
                if timestamp:
                    sql_cmd = "select count(*) from " + kpi_name + " where timestamp<\"" + \
                    str(timestamp) + "\" and cell_id=\"" + str(cell_id) +"\""
                else:
                    sql_cmd = "select count(*) from " + kpi_name + " where cell_id=\"" + str(cell_id) +"\""
            else:
                if timestamp:
                    sql_cmd = "select count(*) from " + kpi_name + " where timestamp<\"" + \
                    str(timestamp) + "\""
                else:
                    sql_cmd = "select count(*) from " + kpi_name

            # print sql_cmd
            value = self.__db_query(sql_cmd)

            if value:
                return str(value)

            return None 

        elif kpi_name.endswith('TPUT'):
            if cell_id:
                if timestamp:
                    sql_cmd = "select value from " + kpi_name + " where timestamp<\"" + \
                    str(timestamp) + "\" and cell_id=\"" + str(cell_id) +"\" order by id desc limit 1"
                else:
                    sql_cmd = "select value from " + kpi_name + " where cell_id=\"" + \
                    str(cell_id) +"\" order by id desc limit 1"
            else:
                if timestamp:
                    sql_cmd = "select value from " + kpi_name + " where timestamp<\"" + \
                    str(timestamp) + "\" order by id desc limit 1"
                else:
                    sql_cmd = "select value from " + kpi_name + " order by id desc limit 1"

            # print sql_cmd
            value = self.__db_query(sql_cmd)

            if value:
                return str(value)

            return None 
       

    def remote_query_kpi(self, kpi_name, phone_model, operator, gps, timestamp):
        """
        Query the remote cloud for the KPI

        :param kpi_name: The KPI to be queried
        :type kpi_name: string
        :param phone_model: The the phone model
        :type phone_model: string
        :param operator: The network operator
        :type operator: string
        :param gps: The GPS coordinate
        :type gps: string
        :param timestamp: The timestamp of the KPI. 
        :type timestamp: datetime
        :returns: The KPI value, or None if the KPI is not available
        """

        #TODO: Implement the query of remote database
        return None


    def set_periodicity(self, kpi_showname, periodicity):
        """
        Set periodicity of the analyzer

        :param kpi_showname: The KPI to be queried, this is the showname
        :type kpi_showname: string
        :param periodicity: periodicity (s,m,h,d repsents scale of seconds, minutes, hours, days)
        :type periodicity: string
        """
        try:
            kpi_name = kpi_showname.replace('.', '_')
            if periodicity.isdigit():
                self.__periodicity[kpi_name] = int(periodicity)
            elif periodicity.endswith('s'):
                self.__periodicity[kpi_name] = int(periodicity[:-1])
            elif periodicity.endswith('m'):
                self.__periodicity[kpi_name] = int(periodicity[:-1])*60
            elif periodicity.endswith('h'):
                self.__periodicity[kpi_name] = int(periodicity[:-1])*60*60
            elif periodicity.endswith('d'):
                self.__periodicity[kpi_name] = int(periodicity[:-1])*60*60*24
            self.__last_updated[kpi_name] = None
            self.log_info("Priority set for "+kpi_showname+': '+periodicity)
            return True
        except:
            self.log_info("Priority set failed for "+kpi_showname+': '+periodicity)
            return False

    def set_cell(self, kpi_showname, cell):
        """
        Set periodicity of the analyzer

        :param kpi_showname: The KPI to be queried, this is the showname
        :type kpi_showname: string
        :param cell: cell (s,m,h,d repsents scale of seconds, minutes, hours, days)
        :type cell: string
        """
        try:
            kpi_name = kpi_showname.replace('.', '_')
            self.__logcell[kpi_name] = cell
            self.log_info("Logging cell set for "+kpi_showname+': '+str(cell))
            return True
        except:
            self.log_info("Logging cell failed for "+kpi_showname+': '+periodicity)
            return False


    def store_kpi(self, kpi_name, kpi_value, timestamp, cur_location=None):
        """
        Store the KPIs to the local database

        :param kpi_name: The KPI to be queried
        :type kpi_name: string
        :param kpi_value: The value of KPI or a dict {attribute <type: str>: value <type: str>}
        :type kpi_value: string
        :param timestamp
        :type timestamp: datetime
        """
        if not self.__db_enabled:
            self.log_warning("Database is not enabled.")
            return True

        # try:
        phone_info = self.__get_phone_model()
        operator_info = self.__get_operator_info()
        # cur_location = self.__get_current_gps()
        cell_id = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_id()
        cell_id = cell_id if cell_id else "None"
        tac = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_tac()
        tac = tac if tac else "None"

        downlink_frequency = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_downlink_frequency()
        downlink_frequency = downlink_frequency if downlink_frequency else "None"

        uplink_frequency = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_uplink_frequency()
        uplink_frequency = uplink_frequency if uplink_frequency else "None"

        downlink_bandwidth = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_downlink_bandwidth()
        downlink_bandwidth = downlink_bandwidth if downlink_bandwidth else "None"

        uplink_bandwidth = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_uplink_bandwidth()
        uplink_bandwidth = uplink_bandwidth if uplink_bandwidth else "None"

        allowed_access = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_allowed_access()
        allowed_access = allowed_access if allowed_access else "None"

        band_indicator = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_band_indicator()
        band_indicator = band_indicator if band_indicator else "None"

        #FIXME: How to handle the missing GPS location?
        if not cur_location:
            cur_location = ("None", "None")

        if isinstance(kpi_value, str) or isinstance(kpi_value, int):
            sql_cmd = "insert into " + kpi_name + "(value, timestamp," \
                      "op, phone_model, gps, cell_id, tai_id, dl_freq, ul_freq, dl_bw, ul_bw," \
                      "allowed_access, band_id) values(\"" + \
                      str(kpi_value) + "\"," + "\"" + str(timestamp) \
                      + "\"," + "\"" + operator_info \
                      + "\"," + "\"" + phone_info \
                      + "\"," + "\"" + str(cur_location[0])+"|"+str(cur_location[1]) \
                      + "\"," + "\"" + str(cell_id) \
                      + "\"," + "\"" + str(tac) \
                      + "\"," + "\"" + str(downlink_frequency) \
                      + "\"," + "\"" + str(uplink_frequency) \
                      + "\"," + "\"" + str(downlink_bandwidth) \
                      + "\"," + "\"" + str(uplink_bandwidth) \
                      + "\"," + "\"" + str(allowed_access) \
                      + "\"," + "\"" + str(band_indicator) \
                      + "\")"
        else:
            idx_str = ""
            value_str = ""
            for attribute in kpi_value:
                idx_str += (attribute + ', ')
                value_str += ("\"" + str(kpi_value[attribute]) + "\"," )
            sql_cmd = "insert into " + kpi_name + "(" + idx_str + \
                      " timestamp, op, phone_model, gps, cell_id, tai_id, dl_freq, ul_freq, dl_bw, ul_bw," \
                      "allowed_access, band_id) values(" + value_str + "\""+ str(timestamp) \
                      + "\"," + "\"" + operator_info \
                      + "\"," + "\"" + phone_info \
                      + "\"," + "\"" + str(cur_location[0])+"|"+str(cur_location[1]) \
                      + "\"," + "\"" + str(cell_id) \
                      + "\"," + "\"" + str(tac) \
                      + "\"," + "\"" + str(downlink_frequency) \
                      + "\"," + "\"" + str(uplink_frequency) \
                      + "\"," + "\"" + str(downlink_bandwidth) \
                      + "\"," + "\"" + str(uplink_bandwidth) \
                      + "\"," + "\"" + str(allowed_access) \
                      + "\"," + "\"" + str(band_indicator) \
                      + "\")"
        # print(sql_cmd)
        if is_android:
            self.__db.execSQL(sql_cmd)
        else:
            self.__db.execute(sql_cmd)
            self.__conn.commit()

        self.__log_kpi(kpi_name, timestamp, cell_id, kpi_value)
        return True
        # except BaseException:  # TODO: raise warnings
            # return False

    def __log_kpi(self, kpi_name, timestamp, cell_id, kpi_value):
        """
        :param kpi_name: The KPI to be queried
        :type kpi_name: string
        :param timestamp
        :type timestamp: datetime
        :param cell_id: updated kpi cell id
        :type cell_id: string
        """

        if kpi_name in self.__last_updated:
            # if logging cell is specified, check whether cell id are the same
            if not self.__logcell[kpi_name] or self.__logcell[kpi_name] and self.__logcell[kpi_name] == str(cell_id):
                kpi_showname = kpi_name.replace('_', '.')
                # if periodicity mode enabled, check whether time gap is longer enough
                if not self.__last_updated[kpi_name] or (timestamp - self.__last_updated[kpi_name]).total_seconds() > self.__periodicity[kpi_name]:
                    self.__last_updated[kpi_name] = timestamp
                    if kpi_name.endswith('_LOSS') or kpi_name.endswith('_BLER'):
                        self.log_info(str(timestamp) + ': '+ str(kpi_showname) + '=' + str(kpi_value) + '%')
                    elif kpi_name.endswith('_TPUT'):
                        self.log_info(str(timestamp) + ': '+ str(kpi_showname) + '=' + str(kpi_value) + 'bps')
                    elif kpi_name.endswith('_LATENCY') or kpi_name.endswith('_HOL'):
                        self.log_info(str(timestamp) + ': '+ str(kpi_showname) + '=' + str(kpi_value) + 'ms')
                    elif kpi_name.endswith('_PREDICTION'):
                        self.log_info(str(timestamp) + ': '+ str(kpi_showname) + '=Triggered')
                    else:
                        self.log_info(str(timestamp) + ': '+ str(kpi_showname) + '=' + str(self.local_query_kpi(kpi_name)))

        # check the stats updated with instance value
        if kpi_name.endswith('SUC') or kpi_name.endswith('FAILURE'):
            kpi_name=kpi_name.replace('SUC', 'SR')
            kpi_name=kpi_name.replace('FAILURE', 'SR')
            if kpi_name in self.__last_updated:
                if not self.__logcell[kpi_name] or self.__logcell[kpi_name] and self.__logcell[kpi_name] == str(cell_id):
                    kpi_showname = kpi_name.replace('_', '.')
                    if not self.__last_updated[kpi_name] or (timestamp - self.__last_updated[kpi_name]).total_seconds() > self.__periodicity[kpi_name]:
                        self.__last_updated[kpi_name] = timestamp
                        kpi_showname = kpi_name.replace('_', '.')
                        self.log_info(str(timestamp) + ': '+ str(kpi_showname) + '=' + str(self.local_query_kpi(kpi_name)))

    def __upload_kpi_thread(self,e):
        """
        Internal thread to upload the KPI
        """

        while True:
            if KpiAnalyzer.pending_upload_task:
                while True:
                    activeNetworkInfo = ConnectivityManager.getActiveNetworkInfo()
                    if activeNetworkInfo and activeNetworkInfo.isConnected():
                        break
                    e.wait(1)

                while KpiAnalyzer.pending_upload_task:
                    item =  KpiAnalyzer.pending_upload_task.popleft()
                    # self.__upload_kpi_async(item[0],item[1])
                    while not self.__upload_kpi_async(item[0],item[1],item[2]):
                        e.wait(5)
            e.wait(5)

    def __upload_kpi_async(self,kpi_name, kpi_value, cur_location):
        """
        Upload the KPI value to the cloud

        :param kpi_name: The KPI to be queried
        :type kpi_name: string
        :param kpi_value: The value of KPI
        :type kpi_value: string
        """
        self.log_debug("uploading kpi: "+kpi_name)
        if is_android:
            phone_info = self.__get_phone_model()
            operator_info = self.__get_operator_info()
            # cur_location = self.__get_current_gps()
            cell_id = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_id()
            cell_id = cell_id if cell_id else "None"
            tac = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_tac()
            tac = tac if tac else "None"

            downlink_frequency = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_downlink_frequency()
            downlink_frequency = downlink_frequency if downlink_frequency else ""

            uplink_frequency = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_uplink_frequency()
            uplink_frequency = uplink_frequency if uplink_frequency else ""

            downlink_bandwidth = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_downlink_bandwidth()
            downlink_bandwidth = downlink_bandwidth if downlink_bandwidth else ""

            uplink_bandwidth = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_uplink_bandwidth()
            uplink_bandwidth = uplink_bandwidth if uplink_bandwidth else ""

            allowed_access = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_allowed_access()
            allowed_access = allowed_access if allowed_access else ""

            band_indicator = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_band_indicator()
            band_indicator = band_indicator if band_indicator else ""

            #FIXME: How to handle the missing GPS location?
            if not cur_location:
                cur_location = "None"

            for item in kpi_value:
                if not kpi_value[item]:
                    kpi_value[item] = "None"

            httpClient = None
            try:
                postdata = {'Phone_model': phone_info, 
                           'operator': operator_info,
                           'GPS': str(cur_location[0])+"|"+str(cur_location[1]),
                           'Time': time.time(),
                           'Cell_ID': str(cell_id),
                           'TAI_ID' : str(tac),
                           'DL_Freq': str(downlink_frequency),
                           'UL_Freq': str(uplink_frequency),
                           'DL_Bandwidth': str(downlink_bandwidth),
                           'UL_Bandwidth': str(uplink_bandwidth),
                           'Allowed_access': str(allowed_access),
                           'Band indicator': str(band_indicator),
                           'KPI_type' : kpi_name,
                           'KPI_val': kpi_value,
                            }
                                           
                # url = 'http://34.213.149.155/postdata/'
                url = 'http://knowledge-map.xyz/postdata/'

                # self.log_debug(str(postdata))


                jdata = json.dumps(postdata)  
                req = urllib.request.Request(url, jdata)  
                response = urllib.request.urlopen(req) 

                self.log_debug("New KPI uploaded:" + kpi_name)
                if httpClient:
                    httpClient.close()
                return True
            except Exception as e:
                # import traceback
                # self.log_error(str(traceback.format_exc())) 
                self.log_warning("Fail to upload the KPI: "+ kpi_name)
                if httpClient:
                    httpClient.close()
                return False
                
        else:
            self.log_info("New KPI (uploading skipped): "+kpi_name)
            return True


    def upload_kpi(self,kpi_name, kpi_value):
        """
        Upload the KPI value to the cloud

        :param kpi_name: The KPI to be queried
        :type kpi_name: string
        :param kpi_value: The value of KPI
        :type kpi_value: string
        """
        # self.log_info("New KPI: " + kpi_name)
        cur_location = self.__get_current_gps()
        KpiAnalyzer.pending_upload_task.append((kpi_name,kpi_value,cur_location))
        
    def __get_phone_model(self):
        if is_android:
            #TODO: Optimization, avoid repetitive calls
            res = mi2app_utils.get_phone_manufacturer()+"-"+mi2app_utils.get_phone_model()
            # self.log_debug("Phone model: "+res)
            return res
        else:
            return self.__phone_model

    def __get_operator_info(self):
        if is_android:
            #TODO: Optimization, avoid repetitive calls
            return mi2app_utils.get_operator_info()
        else:
            self.__op = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_op()
            return self.__op

    def __get_current_gps(self):
        if is_android:
            location = mi2app_utils.get_current_location()
            # self.log_debug("Current location: "+str(location)) 
            return location
        else:
            return ""

    def set_phone_model(self, phone_model):
        """
        Set phone model
        :param phone_model: string
        :return:
        """
        self.__phone_model = phone_model

    def set_operator(self, operator):
        """
        Set operator
        :param operator: string
        :return:
        """
        self.__op = operator

    

