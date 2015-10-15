#!/usr/bin/python
# Filename: analyzer.py
"""
A event-driven analyzer abstraction, 
including low-level msg filter and high-level analyzer

Author: Yuanjie Li
"""

"""
    Analyzer 2.0 development plan

        Step 1: A query() interface with SQLlite

            - Backward compatability with Analyzer 1.0

        Step 2: replace logging with customized logger

        Step 3: A global analyzer repo to guarantee consistency

"""

from ..element import Element, Event
import logging

from jnius import autoclass

def setup_logger(logger_name, log_file, level=logging.INFO):
    '''Setup the analyzer logger.

    NOTE: All analyzers share the same logger.

    :param logger_name: logger to be setup.
    :param log_file: the file to save the log.
    :param level: the loggoing level. The default value is logging.INFO.
    '''


    # FIXME: python's logging module does not work well on Android
    l = logging.getLogger(logger_name)
    formatter = logging.Formatter('%(message)s')
    streamHandler = logging.StreamHandler()
    streamHandler.setFormatter(formatter)

    l.setLevel(level)
    l.addHandler(streamHandler)

    if log_file!="":
        fileHandler = logging.FileHandler(log_file, mode='w')
        fileHandler.setFormatter(formatter)
        l.addHandler(fileHandler)  
    l.disabled = False    

class Analyzer(Element):
    """A base class for all the analyzers
    """

    def __init__(self):
        Element.__init__(self)
        self.source=None    #trace source collector
        #callback when source pushes messages
        #FIXME: looks redundant with the from_list
        self.source_callback=[]    

        #setup the logs
        self.set_log("",logging.INFO)

        #setup the database
        activity = autoclass('org.renpy.android.PythonActivity') #FIXME: does activity work for Analyzer?
        self.__db = activity.mActivity.openOrCreateDatabase('MobileInsight.db',0,None)
        self.__setup_db()


    def __setup_db(self):
        """
        Setup two tables for the analyzer: profile and status
        """
        self.__db.execSQL("CREATE TABLE IF NOT EXISTS "
            + self.__class__.__name__ + "Profile"
            + "(timestamp,ID,profile)")

        self.__db.execSQL("CREATE TABLE IF NOT EXISTS "
            + self.__class__.__name__ + "Status"
            + "(timestamp,status)")

    def query_status(self):
        """
        Query the current status of the analyzer

        NOTE: for advanced analyzers, they may provide more specific query interfaces

        :returns: the latest stauts (String in a analyzer-specific format)

        """
        resultSet = mydatabase.rawQuery("selct status from " + self.__class__.__name__ + "Status"
            + " order by timestamp desc limit 1")
        resultSet.moveToFirst();
        return resultSet.getString(0);

    def set_log(self,logpath,loglevel=logging.INFO):
        """
        Set the logging in analyzers.
        All the analyzers share the same logger.

        :param logpath: the file path to save the log
        :param loglevel: the level of the log. The default value is logging.INFO.
        """
        self.__logpath=logpath
        self.__loglevel=loglevel
        setup_logger('automator_logger',self.__logpath,self.__loglevel)
        self.logger=logging.getLogger('automator_logger')
  
    def set_source(self,source):
        """
        Set the source of the trace. 
        The messages from the source will drive the analysis.

        :param source: the source trace collector
        :param type: trace collector
        """

        #Bottom-up setting: the included analyzers should be evaluated first, then top analyzer
        # FIXME: current implementation is dangerous, any from/to analyzer can change source!
        # FIXME: if looped analyzer dependency exists, dead lock happens here!!!

        #Recursion for analyzers it depends on
        for analyzer in self.from_list:
            analyzer.set_source(source)
            
        if self.source != None:
            self.source.deregister(self)
        self.source = source
        source.register(self)

    def add_source_callback(self,callback):
        """
        Add a callback function to the analyzer. 
        When a message arrives, the analyzer will trigger the callbacks for analysis. 

        :param callback: the callback function to be added
        """
        if callback not in self.source_callback:
            self.source_callback.append(callback)

    def rm_source_callback(self,callback):
        """
        Delete a callback function to the analyzer. 

        :param callback: the callback function to be deleted
        """
        if callback in self.source_callback:
            self.source_callback.remove(callback)

    #TODO: depreciate this moudle. Replace with a registeration based approach
    def include_analyzer(self,analyzer,callback_list):
        """
        Declares the dependency from other analyzers.
        Once declared, the current analyzer will receive events 
        from other analyzers, then trigger functions in callback_list

        :param analyzer: the analyzer to depend on
        :param callback_list: a list of callback functions. They will be triggered when an event from analyzer arrives
        """
        #WARNING: if analyzer exits, its callback_list would be overwritten!!!
        self.from_list[analyzer]=callback_list
        #Add the analyzer to the to_list
        if self not in analyzer.to_list:
            analyzer.to_list.append(self)

    def exclude_analyzer(self,analyzer):
        """
        Remove the dependency from the ananlyzer

        :param analyzer: the analyzer to not depend on
        """
        if self in analyzer.to_list:
            analyzer.to_list.remove(self)
        del self.from_list[analyzer]

    def recv(self,module,event):
        """
        Handle the received events.
        This is an overload member from Element

        :param module: the analyzer/trace collector who raise the event
        :param event: the event to be raised
        """
        if module==self.source:
            for f in self.source_callback:
                f(event)
        else:
            for f in self.from_list[module]:
                f(event)

