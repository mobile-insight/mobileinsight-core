#!/usr/bin/python
# Filename: msg_logger.py
"""
A simple message dumper in stdio and/or file

Author: Yuanjie Li, Zengwen Yuan
"""

from .analyzer import *

import xml.etree.ElementTree as ET
import io
import datetime
import json

__all__ = ["MsgLogger"]


class MsgLogger(Analyzer):
    """
    A simple dumper to print messages
    """

    def __init__(self):
        Analyzer.__init__(self)
        # a message dump has no analyzer in from/to_list
        # it only has a single callback for the source

        self.__msg_log = []  # in-memory message log
        self.add_source_callback(self.__dump_message)
        self.decode_type = 0
        self._save_file_path = None
        self._save_file = None
        self._dump_type = self.ALL

    def __del__(self):
        if self._save_file:
            self._save_file.close()

    def set_dump_type(self, dump_type):
        """
        Specify if dump message to stdio and/or file

        :param dump_type: the dump type
        :type dump_type: STDIO_ONLY, FILE_ONLY, ALL
        """
        if dump_type != self.STDIO_ONLY \
                and dump_type != self.FILE_ONLY \
                and dump_type != self.ALL:
            return
        self._dump_type = dump_type

    def set_decode_format(self, msg_format):
        """
        Configure the format of decoded message. If not set, the message will not be decoded

        :param msg_format: the format of the decoded message
        :type msg_format: NO_DECODING, XML, JSON or DICT
        """
        if msg_format != self.NO_DECODING \
                and msg_format != self.XML \
                and msg_format != self.JSON \
                and msg_format != self.DICT:
            return

        self.decode_type = msg_format

    def save_decoded_msg_as(self, filepath):
        """
        Save decoded messages as a plain-text file.
        If not called, by default MsgLogger will not save decoded results as file.

        :param filepath: the path of the file to be saved
        :type filepath: string
        """

        if not isinstance(filepath, str):
            return

        self._save_file_path = filepath

        try:
            if self._save_file:
                self._save_file.close()
                self._save_file = None

            self._save_file = open(self._save_file_path, 'w')
        except OSError as err:
            self.log_error("I/O error: {0}".format(err))

    def __dump_message(self, msg):
        """
        Print the received message

        :param msg: the received message
        """
        if not msg.data:
            return
        # self.__msg_log.append(msg)
        date = datetime.datetime.fromtimestamp(
            msg.timestamp).strftime('%Y-%m-%d %H:%M:%S.%f')
        # self.log_info(date+':'+msg.type_id)
        decoded_msg = ""
        if self.decode_type == self.XML:
            decoded_msg = msg.data.decode_xml()
        elif self.decode_type == self.JSON:
            decoded_msg = msg.data.decode_json()
            try:
                json_obj = json.loads(decoded_msg)
            except BaseException:
                return

            if msg.type_id != 'LTE_RRC_OTA_Packet':
                self.log_info(json_obj['timestamp'] + '  ' + msg.type_id)

            try:
                parse = json_obj['Msg']['msg']['packet']['proto'][3]
                self.log_info(
                    json_obj['timestamp'] +
                    '  ' +
                    msg.type_id +
                    ':' +
                    parse['field']['@showname'] +
                    '/' +
                    parse['field']['field'][1]['field'][1]['field']['@name'])
            except BaseException:
                pass
        elif self.decode_type == self.DICT:
            decoded_msg = msg.data.decode()
        else:
            return

        if self._dump_type == self.STDIO_ONLY or self._dump_type == self.ALL:
            self.log_info(decoded_msg)
        if (self._dump_type == self.FILE_ONLY or self._dump_type == self.ALL) \
                and self._save_file:
            self._save_file.write(str(decoded_msg) + '\n')

    # Decoding scheme

    NO_DECODING = 0
    XML = 1
    JSON = 2
    DICT = 3

    # Dump type
    STDIO_ONLY = 4
    FILE_ONLY = 5
    ALL = 6

    def set_decoding(self, decode_type):
        """
        Specify how to decode the messages

        :param decode_type: specify how to decode messages. It can be MsgLogger.NO_DECODING, MsgLogger.XML or MsgLogger.JSON
        :type decode_type: int
        """

        self.decode_type = decode_type
