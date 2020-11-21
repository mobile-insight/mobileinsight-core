#!/usr/bin/python
# Filename: wcdma_rrc_analyzer.py
"""
A WCDMA (3G) RRC analyzer.

Author: Yuanjie Li, Zhehui Zhang
"""

import xml.etree.ElementTree as ET
from .analyzer import *
from .state_machine import *
from .protocol_analyzer import *
import timeit

from .profile import Profile,ProfileHierarchy

__all__=["WcdmaRrcAnalyzer"]

class WcdmaRrcAnalyzer(ProtocolAnalyzer):

    """
    A protocol ananlyzer for WCDMA (3G) Radio Resource Control (RRC) protocol.
    """

    def __init__(self):

        # self.log_info("Initialing WcdmaRrcAnalyzer..")

        ProtocolAnalyzer.__init__(self)

        #init packet filters
        self.add_source_callback(self.__rrc_filter)

        #init internal states
        self.__status=WcdmaRrcStatus()    # current cell status
        self.__history={}    # cell history: timestamp -> WcdmaRrcStatus()
        self.__config={}    # cell_id -> WcdmaRrcConfig()
        self.state_machine = self.create_state_machine()
        # print type(self.state_machine)

        #FIXME: change the timestamp
        self.__history[0]=self.__config

        #Temporary structure for holding the config
        self.__config_tmp=WcdmaRrcConfig()

        # self.__profile = Profile(WcdmaRrcProfileHierarchy())

    def set_source(self,source):
        """
        Set the trace source. Enable the WCDMA RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        Analyzer.set_source(self,source)
        #enable WCDMA RRC log
        source.enable_log("WCDMA_RRC_OTA_Packet")
        source.enable_log("WCDMA_RRC_Serv_Cell_Info")
        source.enable_log("WCDMA_RRC_States")

    def create_state_machine(self):
        """
        Declare a RRC state machine

        returns: a StateMachine
        """

        def to_cell_fach(msg):
            if msg.type_id == "WCDMA_RRC_States" and str(msg.data['RRC State']) == 'CELL_FACH':
                return True

        def to_cell_dch(msg):
            if msg.type_id == "WCDMA_RRC_States" and str(msg.data['RRC State']) == 'CELL_DCH':
                return True

        def to_ura_pch(msg):
            if msg.type_id == "WCDMA_RRC_States" and str(msg.data['RRC State']) == 'URA_PCH':
                return True

        def to_cell_pch(msg):
            if msg.type_id == "WCDMA_RRC_States" and str(msg.data['RRC State']) == 'CELL_PCH':
                return True

        def to_idle(msg):
            if msg.type_id == "WCDMA_RRC_States" and str(msg.data['RRC State']) == 'DISCONNECTED':
                return True

        def init_state(msg):
            if msg.type_id == "WCDMA_RRC_States":
                state = 'IDLE' if str(msg.data['RRC State']) == 'DISCONNECTED' else str(msg.data['RRC State'])
                return state

        # def idle_to_dch(msg):
        #     for field in msg.data.iter('field'):
        #         if field.get('name') == "rrc.rrcConnectionSetup":
        #             return True
        #
        # def dch_to_idle(msg):
        #     for field in msg.data.iter('field'):
        #         if field.get('name') == "rrc.rrcConnectionRelease":
        #             return True

        rrc_state_machine={'URA_PCH': {'CELL_FACH': to_cell_fach, 'CELL_DCH': to_cell_dch},
                       'CELL_PCH': {'CELL_FACH': to_cell_fach},
                       'CELL_DCH': {'URA_PCH': to_ura_pch, 'CELL_PCH': to_cell_pch, 'CELL_FACH': to_cell_fach, 'IDLE': to_idle},
                       'CELL_FACH': {'URA_PCH': to_ura_pch, 'CELL_PCH': to_cell_pch, 'CELL_DCH': to_cell_dch, 'IDLE': to_idle},
                       'IDLE': {'CELL_DCH': to_cell_dch, 'CELL_FACH': to_cell_fach}}

        return StateMachine(rrc_state_machine, init_state)

    def __rrc_filter(self,msg):
        
        """
        Filter all WCDMA RRC packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """

        if msg.type_id == "WCDMA_RRC_Serv_Cell_Info":

            # self.log_info("Find One WCDMA_RRC_Serv_Cell_Info")

            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg=Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_serv_cell(raw_msg)

        elif msg.type_id == "WCDMA_RRC_States":

            # self.log_info("Find One WCDMA_RRC_States")

            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            self.__callback_rrc_state(log_item_dict)
            raw_msg = Event(msg.timestamp, msg.type_id, log_item_dict)
            if self.state_machine.update_state(raw_msg):
                self.log_info("WCDMA state: " + self.state_machine.get_current_state())

        elif msg.type_id == "WCDMA_RRC_OTA_Packet":

            # self.log_info("Find One WCDMA_RRC_OTA_Packet")

            log_item = msg.data.decode()
            log_item_dict = dict(log_item) 
            log_xml = None
            # if log_item_dict.has_key('Msg'):
            if 'Msg' in log_item_dict:
                # log_xml = ET.fromstring(log_item_dict['Msg'])
                log_xml = ET.XML(log_item_dict['Msg'])
            else:
                return

            #Convert msg to xml format
            # log_xml = ET.fromstring(log_item_dict['Msg'])
            xml_msg=Event(msg.timestamp,msg.type_id,log_xml)

            self.__callback_sib_config(xml_msg)
            #TODO: callback RRC

            # Raise event to other analyzers
            # FIXME: the timestamp is incoherent with that from the trace collector
            # e = Event(timeit.default_timer(),self.__class__.__name__,"")
            # self.send(e)
            self.send(xml_msg) #deliver WCDMA signaling messages only (decoded)

    def __callback_rrc_state(self,msg):
        # self.log_info("RRC_State="+str(msg['RRC State'])+" Timestamp="+str(msg['timestamp']))
        rrc_state = {}
        rrc_state['RRC State'] = str(msg['RRC State'])
        rrc_state['Timestamp'] = str(msg['timestamp'])
        self.broadcast_info('RRC_STATE',rrc_state)

    def __callback_serv_cell(self,msg):
        """
        A callback to update current cell status

        :param msg: the RRC messages with cell status
        """
        status_updated = False
        if not self.__status.inited():
            #old yet incomplete config would be discarded
            self.__status.freq=msg.data['Download RF channel number']
            self.__status.id=msg.data['Cell ID']
            self.__status.lac=msg.data['LAC']
            self.__status.rac=msg.data['RAC']
            status_updated = True

            # if self.__status.inited():
            #     #push the config to the library
            #     cur_pair=(self.__status.id,self.__status.freq)
            #     # if not self.__config.has_key(cur_pair):
            #     if cur_pair not in self.__config:
            #         self.__config[cur_pair] = self.__config_tmp
            #         self.__config[cur_pair].status = self.__status
            #         # self.log_info(self.__status.dump())
            #     else:
            #         #FIXME: merge two config? Critical for itner-freq
            #         for item in self.__config_tmp.sib.inter_freq_config:
            #             # if not self.__config[cur_pair].sib.inter_freq_config.has_key(item):
            #             if item not in self.__config[cur_pair].sib.inter_freq_config:
            #                 self.__config[cur_pair].sib.inter_freq_config[item]\
            #                 =self.__config_tmp.sib.inter_freq_config[item]
            #                 # self.log_info(self.__status.dump())
        else:
            #if new config arrives, push new one to the history

            if self.__status.freq!=msg.data['Download RF channel number'] \
            or self.__status.id!=msg.data['Cell ID'] \
            or self.__status.lac!=msg.data['LAC'] \
            or self.__status.rac!=msg.data['RAC']:
                self.__status=WcdmaRrcStatus()
                self.__status.freq=msg.data['Download RF channel number']
                self.__status.id=msg.data['Cell ID']
                self.__status.lac=msg.data['LAC']
                self.__status.rac=msg.data['RAC']
                self.__history[msg.timestamp]=self.__status
                #Initialize a new config
                self.__config_tmp=WcdmaRrcConfig()

                status_updated = True

        if status_updated:
            self.log_info(self.__status.dump())

    def __callback_sib_config(self,msg):
        """
        A callback to extract configurations from System Information Blocks (SIBs), 
        including the radio asssement thresholds, the preference settings, etc.

        :param msg: RRC SIB messages
        """
        # if not self.__status.id: #serving cell is not initialized yet
        #     return

        for field in msg.data.iter('field'):

            #Cell Identity: should be done in __callback_serv_cell
            if field.get('name') == "rrc.cellIdentity":
                cellId = int(field.get('value')[0:-1],16)
                # self.__status.dump()
                if not self.__status.inited():
                    self.__status.id = cellId
                    if self.__status.inited():
                        #push the config to the library
                        cur_pair = (self.__status.id,self.__status.freq)
                        self.__config[cur_pair] = self.__config_tmp
                        self.__config[cur_pair].__status = self.__status
                elif self.__status.id != cellId:
                    self.__status = WcdmaRrcStatus()
                    self.__status.id = cellId
                    self.__history[msg.timestamp] = self.__status
                    #Initialize a new config
                    self.__config_tmp = WcdmaRrcConfig()

            #serving cell info
            if field.get('name') == "rrc.utra_ServingCell_element": 
                field_val = {}

                #Default value setting
                #FIXME: set default to those in TS25.331
                field_val['rrc.priority'] = None    #mandatory
                field_val['rrc.threshServingLow'] = None    #mandatory
                field_val['rrc.s_PrioritySearch1'] = None    #mandatory
                field_val['rrc.s_PrioritySearch2'] = 0    #optional

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                
                serv_config = WcdmaRrcSibServ(
                    int(field_val['rrc.priority']),
                    int(field_val['rrc.threshServingLow'])*2,
                    int(field_val['rrc.s_PrioritySearch1'])*2,
                    int(field_val['rrc.s_PrioritySearch2']))
                
                if not self.__status.inited():
                    self.__config_tmp.sib.serv_config = serv_config
                else:
                    cur_pair = (self.__status.id,self.__status.freq)
                    if cur_pair not in self.__config:
                        self.__config[cur_pair] = WcdmaRrcConfig()
                        self.__config[cur_pair].status=self.__status

                    self.__config[cur_pair].sib.serv_config = serv_config

                if self.__status.inited():
                    self.profile.update("WcdmaRrcProfile:"+str(self.__status.id)+"_"+str(self.__status.freq)+".idle.serv_config",
                        {'priority':field_val['rrc.priority'],
                         'threshserv_low':str(int(field_val['rrc.threshServingLow'])*2),
                         's_priority_search1':str(int(field_val['rrc.s_PrioritySearch1'])*2),
                         's_priority_search2':field_val['rrc.s_PrioritySearch2']
                         })


            #intra-freq info
            if field.get('name') == "rrc.cellSelectReselectInfo_element":
                field_val = {}

                #default value based on TS25.331
                field_val['rrc.s_Intrasearch'] = 0
                field_val['rrc.s_Intersearch'] = 0
                field_val['rrc.q_RxlevMin'] = None #mandatory
                field_val['rrc.q_QualMin'] = None #mandatory
                field_val['rrc.q_Hyst_l_S'] = None #mandatory
                field_val['rrc.t_Reselection_S'] = None #mandatory
                field_val['rrc.q_HYST_2_S'] = None #optional, default=q_Hyst_l_S

                #TODO: handle rrc.RAT_FDD_Info_element (p1530, TS25.331)
                #s-SearchRAT is the RAT-specific threshserv

                #TODO: handle FDD and TDD

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                #TS25.331: if missing, the default value is 4dB (2 here)
                if not field_val['rrc.q_Hyst_l_S']:
                    field_val['rrc.q_Hyst_l_S'] = 2

                #TS25.331: if qHyst-2s is missing, the default is qHyst-1s
                if not field_val['rrc.q_HYST_2_S']:
                    field_val['rrc.q_HYST_2_S'] = field_val['rrc.q_Hyst_l_S']

                if not field_val['rrc.t_Reselection_S']:
                    field_val['rrc.t_Reselection_S'] = 0

                if not field_val['rrc.q_RxlevMin']:
                    field_val['rrc.q_RxlevMin'] = 0


                intra_freq_config = WcdmaRrcSibIntraFreqConfig(
                        int(field_val['rrc.t_Reselection_S']),
                        int(field_val['rrc.q_RxlevMin'])*2,
                        int(field_val['rrc.s_Intersearch'])*2,
                        int(field_val['rrc.s_Intrasearch'])*2,
                        int(field_val['rrc.q_Hyst_l_S'])*2,
                        int(field_val['rrc.q_HYST_2_S'])*2)

                if not self.__status.inited():        
                    self.__config_tmp.sib.intra_freq_config = intra_freq_config
                else:
                    cur_pair = (self.__status.id,self.__status.freq)
                    if cur_pair not in self.__config:
                        self.__config[cur_pair] = WcdmaRrcConfig()
                        self.__config[cur_pair].status=self.__status
                    self.__config[cur_pair].sib.intra_freq_config = intra_freq_config

                if self.__status.inited():
                    self.profile.update("WcdmaRrcProfile:"+str(self.__status.id)+"_"+str(self.__status.freq)+".idle.intra_freq_config",
                        {'tReselection':field_val['rrc.t_Reselection_S'],
                         'q_RxLevMin':str(int(field_val['rrc.q_RxlevMin'])*2),
                         's_InterSearch':str(int(field_val['rrc.s_Intrasearch'])*2),
                         's_IntraSearch':str(int(field_val['rrc.s_Intrasearch'])*2),
                         'q_Hyst1':str(int(field_val['rrc.q_Hyst_l_S'])*2),
                         'q_Hyst2':str(int(field_val['rrc.q_HYST_2_S'])*2)
                         })


            #inter-RAT cell info (LTE)
            if field.get('name') == "rrc.EUTRA_FrequencyAndPriorityInfo_element":
                field_val = {}

                #FIXME: set to the default value based on TS36.331
                field_val['rrc.earfcn'] = None
                #field_val['lte-rrc.t_ReselectionEUTRA'] = None
                field_val['rrc.priority'] = None
                field_val['rrc.qRxLevMinEUTRA'] = -140
                #field_val['lte-rrc.p_Max'] = None
                field_val['rrc.threshXhigh'] = None
                field_val['rrc.threshXlow'] = None

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                neighbor_freq = int(field_val['rrc.earfcn'])

                inter_freq_config=WcdmaRrcSibInterFreqConfig(
                                    neighbor_freq,
                                    #float(field_val['lte-rrc.t_ReselectionEUTRA']),
                                    None,
                                    int(field_val['rrc.qRxLevMinEUTRA'])*2,
                                    #float(field_val['lte-rrc.p_Max']),
                                    None,
                                    int(field_val['rrc.priority']),
                                    int(field_val['rrc.threshXhigh'])*2,
                                    int(field_val['rrc.threshXlow'])*2
                                    )
                if not self.__status.inited():
                    self.__config_tmp.sib.inter_freq_config[neighbor_freq] = inter_freq_config
                else:
                    cur_pair = (self.__status.id,self.__status.freq)
                    if cur_pair not in self.__config:
                        self.__config[cur_pair] = WcdmaRrcConfig()
                        self.__config[cur_pair].status=self.__status
                    self.__config[cur_pair].sib.inter_freq_config[neighbor_freq] = inter_freq_config

                if self.__status.inited():
                    self.profile.update("WcdmaRrcProfile:"+str(self.__status.id)+"_"+str(self.__status.freq)+".idle.inter_freq_config:"+str(neighbor_freq),
                        {'rat':'LTE',
                         'freq':str(neighbor_freq),
                         #'tReselection':field_val['lte-rrc.t_ReselectionEUTRA'],
                         'tReselection':'null',
                         'q_RxLevMin':str(int(field_val['rrc.qRxLevMinEUTRA'])*2),
                         #'p_Max':field_val['lte-rrc.p_Max'],
                         'p_Max':'null',
                         'priority':field_val['rrc.priority'],
                         'threshx_high':str(int(field_val['rrc.threshXhigh'])*2),
                         'threshx_low':str(int(field_val['rrc.threshXlow'])*2)
                         })

            #TODO: RRC connection status update

    def get_cell_list(self):
        """
        Get a complete list of cell IDs.

        :returns: a list of cells the device has associated with
        """
        return list(self.__config.keys())

    def get_cell_config(self,cell):
        """
        Return a cell's active/idle-state configuration.
        
        :param cell:  a cell identifier
        :type cell: a (cell_id,freq) pair
        :returns: this cell's active/idle-state configurations
        :rtype: WcdmaRrcConfig
        """
        # if self.__config.has_key(cell):
        if cell in self.__config:
            return self.__config[cell]
        else:
            return None

    def get_cur_cell(self):
        """
        Get current cell's status

        :returns: current cell's status
        :rtype: WcdmaRrcStatus      
        """
        return self.__status

    def get_cur_cell_config(self):
        """
        Get current cell's configuration

        :returns: current cell's status
        :rtype: WcdmaRrcConfig
        """
        cur_pair = (self.__status.id,self.__status.freq)
        # if self.__config.has_key(cur_pair):
        if cur_pair in self.__config:
            return self.__config[cur_pair]
        else:
            return None


    def create_profile_hierarchy(self):

        '''
        Return a Wcdma Rrc ProfileHierarchy (configurations)

        :returns: ProfileHierarchy for WCDMA RRC
        '''
        
        profile_hierarchy = ProfileHierarchy('WcdmaRrcProfile')
        root = profile_hierarchy.get_root()
        status = root.add('status',False) #metadata
        sib = root.add('idle',False) #Idle-state configurations
        active = root.add('active',False) #Active-state configurations

        #Status metadata
        status.add('cell_id',False)
        status.add('freq',False)
        status.add('radio_technology',False)
        status.add('routing_area_code',False)
        status.add('location_area_code',False)
        status.add('bandwidth',False)
        status.add('conn_state',False)

        #Idle-state configurations
        sib_serv = sib.add('serv_config',False) #configuration as the serving cell
        #Per-frequency configurations
        intra_freq_config = sib.add('intra_freq_config',False) #Intra-frequency handoff config
        #TODO: for inter-freq/RAT, should have a mapping from freq/RAT to config
        inter_freq_config = sib.add('inter_freq_config',True) #Inter-frequency/RAT handoff config

        sib_serv.add('priority',False) #cell reselection priority
        sib_serv.add('threshserv_low',False) #cell reselection threshold
        sib_serv.add('s_priority_search1',False) #searching other frequencies
        sib_serv.add('s_priority_search2',False)

        #Intra-frequency handoff parameter: frequency level
        intra_freq_config.add('tReselection',False)
        intra_freq_config.add('q_RxLevMin',False)
        intra_freq_config.add('s_InterSearch',False)
        intra_freq_config.add('s_IntraSearch',False)
        intra_freq_config.add('q_Hyst1',False)
        intra_freq_config.add('q_Hyst2',False)

        #Inter-frequency handoff parameter: frequency level
        inter_freq_config.add('rat',False)
        inter_freq_config.add('freq',False)
        inter_freq_config.add('tReselection',False)
        inter_freq_config.add('q_RxLevMin',False)
        inter_freq_config.add('p_Max',False)
        inter_freq_config.add('priority',False)
        inter_freq_config.add('threshx_high',False)
        inter_freq_config.add('threshx_low',False)

        return profile_hierarchy


    def init_protocol_state(self, msg):
        """
        Determine RRC state at bootstrap

        :returns: current RRC state, or None if not determinable 
        """
        for field in msg.data.iter('field'):
            if field.get('name') == "rrc.rrcConnectionSetup" \
            or field.get('name') == "rrc.radioBearerReconfiguration" \
            or field.get('name') == "rrc.measurementReport_element":
                return 'RRC_DCH'
            elif field.get('name') == "rrc.rrcConnectionRelease":
                return 'RRC_IDLE'
        return None

class WcdmaRrcStatus:
    """
    The metadata of a cell, including its ID, frequency band, location/routing area code, 
    bandwidth, connectivity status, etc.
    """
    def __init__(self):
        self.id = None #cell ID
        self.freq = None #cell frequency
        self.rat = "UTRA" #radio technology
        self.rac = None #routing area code
        self.lac = None #location area code
        self.bandwidth = None #cell bandwidth
        self.conn = False #connectivity status (for serving cell only)

    def dump(self):
        """
        Report the cell status

        :returns: a string that encodes the cell status
        :rtype: string
        """
        # print self.__class__.__name__,self.id,self.freq,self.rat,self.rac,self.lac
        return (self.__class__.__name__ 
            + ' cellID=' + str(self.id)
            + ' frequency=' + str(self.freq)
            + ' RAC=' + str(self.rac)
            + ' LAC=' + str(self.lac)+'\n')

    def inited(self):
        return (self.id and self.freq)


class WcdmaRrcConfig:
    """ 
        Per-cell RRC configurations

        The following configurations should be supported
            - Idle-state
                - Cell reselection parameters
            - Active-state
                - PHY/MAC/PDCP/RLC configuration
                - Measurement configurations
    """
    def __init__(self):
        self.status = WcdmaRrcStatus() #the metadata of this cell
        self.sib = WcdmaRrcSib()    #Idle-state
        self.active = WcdmaRrcActive() #active-state configurations

    def dump(self):
        """
        Report the cell configurations

        :returns: a string that encodes the cell's configurations
        :rtype: string
        """
        return (self.__class__.__name__+'\n'
            + self.status.dump()
            + self.sib.dump()
            + self.active.dump())

    def get_cell_reselection_config(self,cell_meta):

        """
        Given a cell, return its reselection config as a serving cell

        :param cell_meta: a cell identifier
        :type cell_meta: a (cell_id,freq) pair

        :returns: cell reselection configurations
        :rtype: WcdmaRrcReselectionConfig
        """
        if not cell_meta:
            return None

        cell = cell_meta.id
        freq = cell_meta.freq

        if freq == self.status.freq: #intra-freq
            hyst = self.sib.intra_freq_config.q_Hyst1
            return WcdmaRrcReselectionConfig(cell,freq,None,hyst,None,None)
        else:
            #inter-frequency/RAT
            #TODO: cell individual offset (not available in AT&T and T-mobile)
            # if not self.sib.inter_freq_config.has_key(freq):
            if freq not in self.sib.inter_freq_config:
                if (not self.sib.serv_config.priority
                or cell_meta.rat == "UTRA"):
                    #WCDMA reselection without priority
                    hyst = self.sib.intra_freq_config.q_Hyst1
                    return WcdmaRrcReselectionConfig(cell,freq,None,hyst,None,None)
            else:
                freq_config = self.sib.inter_freq_config[freq]
                hyst = self.sib.serv_config.s_priority_search2
                return WcdmaRrcReselectionConfig(cell,freq,freq_config.priority, hyst,
                    freq_config.threshx_high,freq_config.threshx_low)


    def get_meas_config(self,cell_meta):
        """
        Given a cell, return its measurement config from the serving cell.
        Note: there may be more than 1 measurement configuration for the same cell.

        :param cell_meta: a cell identifier
        :type cell_meta: a (cell_id,freq) pair
        :returns: RRC measurement configurations
        :rtype: a list of WcdmaRrcReselectionConfig
        """
        #NOT DONE
        return None


class WcdmaRrcSib:

    """
    Per-cell Idle-state SIB configurations
    """
    def __init__(self):
        #FIXME: init based on the default value in TS25.331
        #configuration as a serving cell (LteRrcSibServ)
        self.serv_config = WcdmaRrcSibServ(None,None,None,None) 
        #Intra-freq reselection config
        self.intra_freq_config = WcdmaRrcSibIntraFreqConfig(0,0,None,None,None,None) 
        #Inter-freq/RAT reselection config. Freq/cell -> WcdmaRrcSibInterFreqConfig
        self.inter_freq_config = {}  

    def dump(self):
        """
        Report the cell SIB configurations

        :returns: a string that encodes the cell's SIB configurations
        :rtype: string
        """
        res = self.serv_config.dump() + self.intra_freq_config.dump()
        for item in self.inter_freq_config:
            res += self.inter_freq_config[item].dump()
        return res


class WcdmaRrcReselectionConfig:
    """
    Per-cell cell reselection configurations
    """
    def __init__(self,cell_id,freq,priority,offset,threshX_High,threshX_Low):
        self.id = cell_id
        self.freq = freq
        self.priority = priority
        self.offset = offset #adjusted offset by considering freq/cell-specific offsets
        self.threshx_high = threshX_High
        self.threshx_low = threshX_Low


class WcdmaRrcSibServ:
    """
    Serving cell's SIB configurations
    """
    def __init__(self,priority,thresh_serv, s_priority_search1,s_priority_search2):
        self.priority = priority #cell reselection priority
        self.threshserv_low = thresh_serv #cell reselection threshold
        self.s_priority_search1 = s_priority_search1 #searching other frequencies
        self.s_priority_search2 = s_priority_search2

    def dump(self):
        """
        Report the serving cell SIB configurations

        :returns: a string that encodes the cell's SIB configurations
        :rtype: string
        """
        return (self.__class__.__name__
            + ' ' + str(self.priority)
            + ' ' + str(self.threshserv_low)
            + ' ' + str(self.s_priority_search1) + '\n')


class WcdmaRrcSibIntraFreqConfig:
    """
    Intra-frequency SIB configurations
    """
    def __init__(self,tReselection,q_RxLevMin,s_InterSearch,s_IntraSearch,q_Hyst1,q_Hyst2):
        #FIXME: individual cell offset
        self.tReselection = tReselection
        self.q_RxLevMin = q_RxLevMin
        self.s_InterSearch = s_InterSearch
        self.s_IntraSearch = s_IntraSearch
        self.q_Hyst1 = q_Hyst1
        self.q_Hyst2 = q_Hyst2

    def dump(self):
        """
        Report the cell SIB configurations

        :returns: a string that encodes the cell's SIB configurations
        :rtype: string
        """
        return (self.__class__.__name__
            + ' ' + str(self.tReselection)
            + ' ' + str(self.q_RxLevMin)
            + ' ' + str(self.s_InterSearch)
            + ' ' + str(self.s_IntraSearch)
            + ' ' + str(self.q_Hyst1)
            + ' ' + str(self.q_Hyst2) + '\n')


class WcdmaRrcSibInterFreqConfig:
    """
    Inter-frequency SIB configurations
    """     
    #FIXME: the current list is incomplete
    #FIXME: individual cell offset
    def __init__(self,freq,tReselection,q_RxLevMin,p_Max,priority,threshx_high,threshx_low):
        self.freq = freq
        self.tReselection = tReselection
        self.q_RxLevMin = q_RxLevMin
        self.p_Max = p_Max
        self.priority = priority
        self.threshx_high = threshx_high
        self.threshx_low = threshx_low

    def dump(self):
        """
        Report the cell SIB configurations

        :returns: a string that encodes the cell's SIB configurations
        :rtype: string
        """
        return (self.__class__.__name__
            + ' ' + str(self.freq)
            + ' ' + str(self.priority)
            + ' ' + str(self.tReselection)
            + ' ' + str(self.p_Max)
            + ' ' + str(self.q_RxLevMin)
            + ' ' + str(self.threshx_high)
            + ' ' + str(self.threshx_low) + '\n')


class WcdmaRrcActive:
    """
    RRC active-state configurations (from RRCReconfiguration 
    and MeasurementControl messsage)
    """

    #TODO: parse measurement control messages
    def __init__(self):
        #TODO: initialize some containers
        pass

    def dump(self):
        return "\n"
