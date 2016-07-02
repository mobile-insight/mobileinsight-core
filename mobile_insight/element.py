#!/usr/bin/python
# Filename: element.py
"""
Basic abstractions for the trace collector and analyzer.

Author: Yuanjie Li
"""

__all__ = ["Event", "Element"]

# Detect if Android is used (for logging format)
is_android=False
try:
    from jnius import autoclass #For Android
    is_android=True
except Exception, e:
    is_android=False

import logging
import time
import datetime as dt

class MyFormatter(logging.Formatter):
    converter=dt.datetime.fromtimestamp
    def formatTime(self, record, datefmt=None):
        ct = self.converter(record.created)
        if datefmt:
            s = ct.strftime(datefmt)
        else:
            t = ct.strftime("%Y-%m-%d %H:%M:%S")
            s = "%s,%03d" % (t, record.msecs)
        return s

def setup_logger(logger_name, log_file, level=logging.INFO):
    '''Setup the analyzer logger.

    NOTE: All analyzers share the same logger.

    :param logger_name: logger to be setup.
    :param log_file: the file to save the log.
    :param level: the loggoing level. The default value is logging.INFO.
    '''

    l = logging.getLogger(logger_name)
    if len(l.handlers)<1:
        # formatter = MyFormatter('%(asctime)s %(message)s',datefmt='%Y-%m-%d,%H:%M:%S.%f')
        formatter = MyFormatter('%(message)s')
        streamHandler = logging.StreamHandler()
        streamHandler.setFormatter(formatter)

        l.setLevel(level)
        l.addHandler(streamHandler)
        l.propagate = False

        if log_file!="":
            fileHandler = logging.FileHandler(log_file, mode='w')
            fileHandler.setFormatter(formatter)
            l.addHandler(fileHandler)  
        l.disabled = False 


class Event(object):
    '''The event is used to trigger the analyzer and perform some actions.

    The event can be raised by a trace collector (a message) or an analyzer.
    An event is a triple of (timestamp, type, data).
    '''
    def __init__(self,timestamp,type_id,data):
        self.timestamp = timestamp
        self.type_id = type_id
        self.data = data

class Element(object):
    '''The parent class to derive trace collectors and analyzers.
    '''

    logger=None

    def __init__(self):
        self.from_list={}   #module that it depends, module->callback
        self.to_list=[] #list of other module that call for this module

        #setup the logs
        self.set_log("")

    def send(self,event):
        """
        Raise an event to all Analyzers in from_analyzer_list

        :param event: the event to be sent
        """
        # A lambda function: input as a callback, output as passing event to this callback
        if isinstance(event,Event):
            self.log_debug(event.type_id)
            G = lambda module: module.recv(self,event)
            map(G, self.to_list)

    def recv(self,module,event):
        """
        Upon receiving an event from module, trigger associated callbacks

        This method should be overwritten by the analyzer and trace collector

        :param module: the module who raises the event
        :param event: the event to be received
        """
        pass

    #logging functions: please use this one

    def set_log(self,logpath,loglevel=logging.INFO):
        """
        Set the logging in analyzers.
        All the analyzers share the same logger.

        :param logpath: the file path to save the log
        :param loglevel: the level of the log. The default value is logging.INFO.
        """
        self.__logpath=logpath
        self.__loglevel=loglevel
        setup_logger('mobileinsight_logger',self.__logpath,self.__loglevel)
        # self.logger=logging.getLogger('mobileinsight_logger')
        Element.logger=logging.getLogger('mobileinsight_logger')


    def log_info(self, msg):

        if is_android:
            Element.logger.info(
                "[b][color=00ff00][INFO][/color] ["
                + self.__class__.__name__+'][/b]: '+msg
                )
        else:
            Element.logger.info(
                "\033[32m\033[1m[INFO]\033[0m\033[0m\033[1m ["
                + self.__class__.__name__+']\033[0m: '+msg
                )


    def log_debug(self, msg):
    
        if is_android:
            Element.logger.debug(
                "[b][color=00ffff][DEBUG][/color] ["
                + self.__class__.__name__+'][/b]: '+msg
                )
        else:
            Element.logger.debug(
                "\033[33m\033[1m[DEBUG]\033[0m\033[0m\033[1m ["
                + self.__class__.__name__+']\033[0m: '+msg)

    def log_warning(self, msg):

        if is_android:
            Element.logger.warning(
                "[b][color=ffff00][WARNING][/color] ["
                + self.__class__.__name__+'][/b]: '+msg
                )
        else:
            Element.logger.warning(
                "\033[1;34m\033[1m[WARNING]\033[0m\033[0m\033[1m ["
                + self.__class__.__name__+']\033[0m: '+msg)

    def log_error(self, msg):

        if is_android:
            Element.logger.error(
                "[b][color=ff0000][ERROR][/color] ["
                + self.__class__.__name__+'][/b]: '+msg
                )
        else:
            Element.logger.error(
                "\033[31m\033[1m[ERROR]\033[0m\033[0m\033[1m ["
                + self.__class__.__name__+']\033[0m: '+msg)

    def log_critical(self, msg):
        if is_android:
            Element.logger.critical(
                "[b][color=ff0000][CRITICAL][/color] ["
                + self.__class__.__name__+'][/b]: '+msg
                )
        else:
            Element.logger.critical(
                "\033[31m\033[1m[CRITICAL]\033[0m\033[0m\033[1m ["
                + self.__class__.__name__+']\033[0m: '+msg)
