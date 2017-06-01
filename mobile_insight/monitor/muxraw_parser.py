# muxraw_parser.py
# parse a muxraw file into strings of ws_dissector input
# sample input for ws_dissector:
# echo -ne
# '\x00\x00\x00\xc8\x00\x00\x00\x09\x40\x01\xBF\x28\x1A\xEB\xA0\x00\x00' |
# LD_LIBRARY_PATH="/data/data/edu.ucla.cs.wing.mobileinsight/files/data/"
# /data/data/edu.ucla.cs.wing.mobileinsight/files/data/android_pie_ws_dissector

import struct
import sys
import subprocess

ANDROID_SHELL = "/system/bin/sh"

#RRC_PAGING_TYPE1 = ['0x8b', '0x3', '0x0', '0x0']
SMS_CP      = ['0x90', '0x1', '0x0', '0x0'] #400             
MM_CM_REQ   = ['0x91', '0x1', '0x0', '0x0'] # 401             
MM_AUTH_REQ = ['0x92', '0x1', '0x0', '0x0'] #402             
GMM_UL      = ['0x93', '0x1', '0x0', '0x0'] #403
                    # (gmm_identity_response)/(gmm_rau_comp)/gmm_service_req
GMM_DL      = ['0x94', '0x1', '0x0', '0x0'] #404
                    # (gmm_rau_accept)/gmm_service_accept
SMS_RP      = ['0x95', '0x1', '0x0', '0x0'] #405
CC_UL       = ['0x9e', '0x1', '0x0', '0x0'] #414
CC_DL       = ['0x9f', '0x1', '0x0', '0x0'] #415
SM_PDP      = ['0xa0', '0x1', '0x0', '0x0'] #416
                    # (modify_pdp_accept/ul)
_2G_RR      = ['0xf4', '0x1', '0x0', '0x0'] #500
                    # (gprs_suspend_req_ul)/rr_ciphermode_command_dl/ul 
_2G_RR_SI   = ['0xf5', '0x1', '0x0', '0x0'] #501
_2G_RR_MEAS = ['0xf6', '0x1', '0x0', '0x0'] #502
_2G_RR_CHNL = ['0xf7', '0x1', '0x0', '0x0'] #503    

LTE_BCCH_BCH = ['0xbc', '0x2', '0x0', '0x0']
LTE_BCCH_DL_SCH = ['0xbd', '0x2', '0x0', '0x0']
LTE_DL_CCCH = ['0xbe', '0x2', '0x0', '0x0']
LTE_DL_DCCH = ['0xbf', '0x2', '0x0', '0x0']
LTE_PCCH = ['0xc0', '0x2', '0x0', '0x0']
LTE_UL_CCCH = ['0xc1', '0x2', '0x0', '0x0']
LTE_UL_DCCH = ['0xc2', '0x2', '0x0', '0x0']
RRC_SI_MIB = ['0xe8', '0x3', '0x0', '0x0']
RRC_SI_SB1 = ['0xe9', '0x3', '0x0', '0x0']
RRC_SI_SB2 = ['0xea', '0x3', '0x0', '0x0']
RRC_SI_SIB1 = ['0xeb', '0x3', '0x0', '0x0']
RRC_SI_SIB2 = ['0xec', '0x3', '0x0', '0x0']
RRC_SI_SIB3 = ['0xed', '0x3', '0x0', '0x0']
RRC_SI_SIB4 = ['0xee', '0x3', '0x0', '0x0']
RRC_SI_SIB5a = ['0xef', '0x3', '0x0', '0x0']
RRC_SI_SIB5b = ['0xf0', '0x3', '0x0', '0x0']
RRC_SI_SIB6 = ['0xf1', '0x3', '0x0', '0x0']
RRC_SI_SIB7 = ['0xf2', '0x3', '0x0', '0x0']
RRC_SI_SIB11 = ['0xf6', '0x3', '0x0', '0x0']
RRC_SI_SIB11_BIS = ['0xf7', '0x3', '0x0', '0x0']
RRC_SI_SIB12 = ['0xf8', '0x3', '0x0', '0x0']
RRC_BCCH_RACH   = ['0x85', '0x3', '0x0', '0x0']
RRC_DL_CCCH     = ['0x86', '0x3', '0x0', '0x0']
RRC_DL_DCCH     = ['0x87', '0x3', '0x0', '0x0']
RRC_PAGING_TYPE1 = ['0x8b', '0x3', '0x0', '0x0']
RRC_CONN_REQ    = ['0x8c', '0x3', '0x0', '0x0'] #908
# RRC_UL_CCCH     = ['0xc0', '0x3', '0x0', '0x0']
RRC_UL_DCCH     = ['0x8d', '0x3', '0x0', '0x0']
RRC_HANDOVERTOUTRANCOMMAND = ['0x8f', '0x3', '0x0', '0x0']
RRC_INTERRATHANDOVERINFO = ['0x90', '0x3', '0x0', '0x0']
EMM_SERVICE_REQUEST = ['0x21', '0x3', '0x0', '0x0']
RRC_SI_SIB18 = ['0xe', '0x4', '0x0', '0x0']
RRC_SI_SIB19 = ['0xf', '0x4', '0x0', '0x0']
RRC_SI_SIB20 = ['0x10', '0x4', '0x0', '0x0']

global_msg_id = [
    # SMS_CP,
    MM_CM_REQ,
    MM_AUTH_REQ,
    GMM_UL,
    GMM_DL,
    # SMS_RP,
    CC_UL,
    CC_DL,
    SM_PDP,
    # _2G_RR,
    # _2G_RR_SI,
    # _2G_RR_MEAS,
    # _2G_RR_CHNL,
    LTE_BCCH_BCH,
    LTE_BCCH_DL_SCH,
    LTE_DL_CCCH,
    LTE_DL_DCCH,
    LTE_PCCH,
    LTE_UL_CCCH,
    LTE_UL_DCCH,
    RRC_SI_MIB,
    RRC_SI_SB1,
    RRC_SI_SB2,
    RRC_SI_SIB1,
    RRC_SI_SIB2,
    RRC_SI_SIB3,
    RRC_SI_SIB4,
    RRC_SI_SIB5a,
    RRC_SI_SIB5b,
    RRC_SI_SIB6,
    RRC_SI_SIB7,
    RRC_SI_SIB11,
    RRC_SI_SIB12,
    RRC_SI_SIB18,
    RRC_SI_SIB19,
    RRC_SI_SIB20,
    RRC_DL_CCCH,
    RRC_DL_DCCH,
    RRC_PAGING_TYPE1,
    RRC_CONN_REQ,
    # RRC_UL_CCCH,
    RRC_UL_DCCH,
    RRC_HANDOVERTOUTRANCOMMAND,
    RRC_INTERRATHANDOVERINFO,
    EMM_SERVICE_REQUEST]




global_ws_id = [
    # 0,
    190,
    190,
    190,
    190,
    # 0,
    190,
    190,
    190,
    # 0,
    # 0,
    # 0,
    # 0,
    203,
    203,
    204,
    201,
    200,
    205,
    202,
    150,
    181,
    182,
    151,
    152,
    153,
    154,
    155,
    155,
    156,
    157,
    161,
    162,
    168,
    169,
    170,
    102,
    103,
    106,
    100,
    # 100,
    101,
    103,
    103,
    250]

muxraw_parser_buff = []  # store bytes in current section
first_header = False


def feed_binary(logger, buff):
    global muxraw_parser_buff
    msg_list = []
    cur_index = 0
    end_index = len(buff)
    # parse file into sections devided by '\0x8f\0x9a\0x9a\0x8d\0x04\0x00'
    header = ['0xac', '0xca', '0x0', '0xff']
    header_magic = ['0x8f', '0x9a', '0x9a', '0x8d', '0x4', '0x0']
    while cur_index != end_index:
        byte = buff[cur_index]
        # byte_value couldn't be print out
        byte_value = struct.unpack('B', byte)[0]
        muxraw_parser_buff.append(hex(byte_value))
        if len(muxraw_parser_buff) > 6:
            if muxraw_parser_buff[-6:-2] == header:
                muxraw_parser_buff = muxraw_parser_buff[:-6]
            if muxraw_parser_buff[-6:] == header_magic:
                res = seek_pstrace_magic(muxraw_parser_buff)
                if res != []:
                    msg_list.append(res)
                muxraw_parser_buff = []
        cur_index += 1
    return msg_list


def decode(logger, raw_msg):
    """
    decode raw_msg and return the typeid, xml (for decoded message)
    """
    msg_id = int(raw_msg[0][3], 16)
    msg = "\\" + "\\".join([j[1:] for j in raw_msg[0]])
    logger.log_info("lizhehan: Receive message: " + msg)
    output = msg
    p = subprocess.Popen(
        "su",
        executable=ANDROID_SHELL,
        shell=True,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE)
    output, err = p.communicate("echo -ne \'" +
                                msg +
                                "\' | LD_LIBRARY_PATH=" +
                                logger.libs_path +
                                ' ' +
                                logger.ws_dissector_path +
                                '\n')
    p.wait()
    return_code, output = commands.getstatusoutput(
        "echo -ne " + msg + " | LD_LIBRARY_PATH=/data/data/edu.ucla.cs.wing.mobileinsight/files/data/ /data/data/edu.ucla.cs.wing.mobileinsight/files/data/android_pie_ws_dissector")
    logger.log_info("lizhehan: Output: " + str(len(output)) + "  |  " + output)
    # FIXME: to be replaced with real message type ID
    # I have msg_id
    return "LTE_RRC_OTA_Packet", output


def last_seek():
    global muxraw_parser_buff
    global first_header
    msg_list = []
    if first_header:
        res = seek_pstrace_magic(muxraw_parser_buff)
        if res != []:
            msg_list.append(res)
    return msg_list


def seek_pstrace_magic(bytes):
    global first_header
    pstrace = []

    if not first_header:
        first_header = True
        return pstrace

    msg_id = bytes[0:4]
    if msg_id in global_msg_id:
        # calculate the length of raw_data
        decimal_high = int(bytes[4], 16)
        decimal_low = int(bytes[5], 16)
        length = decimal_low * 256 + decimal_high
        # print 'hello', length, '\n\n', bytes[:20]
        if length > 0 and length < len(bytes):
            raw_bytes = bytes[6:6 + length]
            raw_data = map(lambda x: x if (x != '0x0') else '0x00', raw_bytes)
            raw_msg = ['0x00'] * 3 + [hex(global_ws_id[global_msg_id.index(msg_id)])] + [
                '0x00'] * 2 + [bytes[5]] + [bytes[4]] + raw_data
            pstrace.append(raw_msg)
            return pstrace
    return pstrace


def parse_muxraw_magic(filename):
    msg_list = []
    # global a
    with open(filename, 'rb') as f:
        f.seek(0)
        byte = f.read(1)
        bytes_current = []  # store bytes in current section

        # parse file into sections devided by '\0x8f\0x9a\0x9a\0x8d\0x04\0x00'
        header = ['0xac', '0xca', '0x0', '0xff']
        header_magic = ['0x8f', '0x9a', '0x9a', '0x8d', '0x4', '0x0']
        while len(byte) == 1 and byte != 'ELF':
            # if a > 25:
            #     return msg_list
            # byte_value couldn't be print out
            byte_value = struct.unpack('B', byte)[0]
            bytes_current.append(hex(byte_value))
            if len(bytes_current) > 6:
                if bytes_current[-6:-2] == header:
                    bytes_current = bytes_current[:-6]
                if bytes_current[-6:] == header_magic:
                    res = seek_pstrace_magic(bytes_current)
                    if res != []:
                        msg_list.append(res)
                        # print 'yes'
                    bytes_current = []
            byte = f.read(1)
        res = seek_pstrace_magic(bytes_current)
        if res != []:
            msg_list.append(res)
            # print 'yes'
    return msg_list


if __name__ == "__main__":

    filename = sys.argv[1]
    # filename = "./test_logs/MDLog1_2016_1119_175748.muxraw"
    # print "Processing ", filename
    # raw_msg_list = parse_muxraw(filename)
    raw_msg_list = parse_muxraw_magic(filename)
    msg_list = []
    # print len(raw_msg_list)

    if raw_msg_list != []:
        for i in raw_msg_list:
            output = "\\" + "\\".join([j[1:] for j in i[0]])
            msg_list.append(output)
            print output
    # print msg_list
    # if raw_msg_list != []:
    #     for msg in raw_msg_list:
    #         print decode(msg)
