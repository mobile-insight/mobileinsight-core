# muxraw_parser.py
# parse a muxraw file into strings of ws_dissector input
# usage muxraw_parser.py MDLog1_2016_0402_183638.muxraw
# output ['\\x00\\x00\\x00\\x6a\\x00\\x00\\x00\\x1e\\x42\\x75\\x8d\\xaa\\x56\\xb8\\x75\\xab\\xb2\\x3a\\xf8\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00'
# , '\\x00\\x00\\x00\\x6a\\x00\\x00\\x00\\x1e\\x42\\x75\\xbb\\x99\\x6e\\xc0\\x55\\xff\\xa0\\x39\\xfc\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00']

import struct
import sys
import commands

is_android=False
try:
    from jnius import autoclass #For Android
    try:
        service_context = autoclass('org.renpy.android.PythonService').mService
        if not service_context:
            service_context = cast("android.app.Activity",
                            autoclass("org.renpy.android.PythonActivity").mActivity)
    except Exception, e:
        service_context = cast("android.app.Activity",
                            autoclass("org.renpy.android.PythonActivity").mActivity)

    is_android=True
except Exception, e:
    #not used, but bugs may exist on laptop
    is_android=False

ANDROID_SHELL = "/system/bin/sh"

def get_cache_dir():
    if is_android:
        return str(service_context.getCacheDir().getAbsolutePath())
    else:
        return ""

def get_files_dir():
    if is_android:
        return str(service_context.getFilesDir().getAbsolutePath())
    else:
        return ""

#RRC_PAGING_TYPE1 = ['0x8b', '0x3', '0x0', '0x0']
LTE_BCCH_BCH	= ['0xbc', '0x2', '0x0', '0x0']
LTE_BCCH_DL_SCH	= ['0xbd', '0x2', '0x0', '0x0']
LTE_DL_CCCH	= ['0xbe', '0x2', '0x0', '0x0']
LTE_DL_DCCH	= ['0xbf', '0x2', '0x0', '0x0']
LTE_PCCH	= ['0xc0', '0x2', '0x0', '0x0']
LTE_UL_CCCH	= ['0xc1', '0x2', '0x0', '0x0']
LTE_UL_DCCH	= ['0xc2', '0x2', '0x0', '0x0']
RRC_SI_MIB	= ['0xe8', '0x3', '0x0', '0x0']
RRC_SI_SB1	= ['0xe9', '0x3', '0x0', '0x0']
RRC_SI_SB2	= ['0xea', '0x3', '0x0', '0x0']
RRC_SI_SIB1	= ['0xeb', '0x3', '0x0', '0x0']
RRC_SI_SIB2	= ['0xec', '0x3', '0x0', '0x0']
RRC_SI_SIB3	= ['0xed', '0x3', '0x0', '0x0']
RRC_SI_SIB4	= ['0xee', '0x3', '0x0', '0x0']
RRC_SI_SIB5a	= ['0xef', '0x3', '0x0', '0x0']
RRC_SI_SIB5b	= ['0xf0', '0x3', '0x0', '0x0']
RRC_SI_SIB6	= ['0xf1', '0x3', '0x0', '0x0']
RRC_SI_SIB7	= ['0xf2', '0x3', '0x0', '0x0']
RRC_SI_SIB11	= ['0xf6', '0x3', '0x0', '0x0']
RRC_SI_SIB11_BIS	= ['0xf7', '0x3', '0x0', '0x0']
RRC_SI_SIB12	= ['0xf8', '0x3', '0x0', '0x0']
RRC_SI_SIB18	= ['0xe', '0x4', '0x0', '0x0']
RRC_SI_SIB19	= ['0xf', '0x4', '0x0', '0x0']
RRC_SI_SIB20	= ['0x10', '0x4', '0x0', '0x0']
RRC_BCCH_RACH	= ['0x85', '0x3', '0x0', '0x0']
RRC_DL_CCCH	= ['0x86', '0x3', '0x0', '0x0']
RRC_DL_DCCH	= ['0x87', '0x3', '0x0', '0x0']
RRC_PAGING_TYPE1	= ['0x8b', '0x3', '0x0', '0x0']
RRC_UL_CCCH	= ['0xc0', '0x3', '0x0', '0x0']
RRC_UL_DCCH	= ['0x8d', '0x3', '0x0', '0x0']
RRC_HANDOVERTOUTRANCOMMAND	= ['0x8f', '0x3', '0x0', '0x0']
RRC_INTERRATHANDOVERINFO	= ['0x90', '0x3', '0x0', '0x0']
EMM_SERVICE_REQUEST	= ['0x21', '0x3', '0x0', '0x0']

#global_msg_id = [RRC_PAGING_TYPE1]
#global_ws_id = [106]

global_msg_id = [LTE_BCCH_BCH,LTE_BCCH_DL_SCH,LTE_DL_CCCH,LTE_DL_DCCH,LTE_PCCH,LTE_UL_CCCH,LTE_UL_DCCH,RRC_SI_MIB,RRC_SI_SB1,RRC_SI_SB2,RRC_SI_SIB1,RRC_SI_SIB2,RRC_SI_SIB3,RRC_SI_SIB4,RRC_SI_SIB5a,RRC_SI_SIB5b,RRC_SI_SIB6,RRC_SI_SIB7,RRC_SI_SIB11,RRC_SI_SIB12,RRC_SI_SIB18,RRC_SI_SIB19,RRC_SI_SIB20,RRC_DL_CCCH,RRC_DL_DCCH,RRC_PAGING_TYPE1,RRC_UL_CCCH,RRC_UL_DCCH,RRC_HANDOVERTOUTRANCOMMAND,RRC_INTERRATHANDOVERINFO,EMM_SERVICE_REQUEST]
global_ws_id = [104,203,102,103,106,100,101,150,181,182,151,152,153,154,155,155,156,157,161,162,168,169,170,102,103,106,100,101,103,103,250]

muxraw_parser_buff = []  # store bytes in current section
def feed_binary(buff):
    msg_list = []
    cur_index = 0
    end_index = len(buff)
    # parse file into sections devided by '\0x8f\0x9a\0x9a\0x8d\0x04\0x00'
    header = ['0xac', '0xca', '0x0', '0xff']
    header_magic = ['0x8f', '0x9a', '0x9a', '0x8d', '0x4', '0x0']
    while cur_index != end_index:
        byte = buff[cur_index]
        byte_value = struct.unpack('B', byte)[0] # byte_value couldn't be print out
        muxraw_parser_buff.append(hex(byte_value))
        if len(muxraw_parser_buff) > 6:
            if muxraw_parser_buff[-6:-2] == header:
                muxraw_parser_buff = muxraw_parser_buff[:-6]
            if muxraw_parser_buff[-6:] == header_magic:
                res = seek_pstrace_magic(muxraw_parser_buff)
                if res != []:
                    msg_list.append(res)
                muxraw_parser_buff = []
        cur_index+=1
    res = seek_pstrace_magic(muxraw_parser_buff)
    if res != []:
        msg_list.append(res)
    return msg_list

def decode(raw_msg):
    """
    decode raw_msg and return the typeid, xml (for decoded message)
    """
    print "raw_msg: " + raw_msg
    msg =  "\\" + "\\".join([j[1:] for j in raw_msg[0]])
    print "msg: " + msg

    """
    FIXME: Replace the following ws_dissector with: 

        libs_path = os.path.join(get_files_dir(),"data")
        ws_dissector_path = os.path.join(libs_path,"android_pie_ws_dissector")
    """
    return_code, output = commands.getstatusoutput("echo -ne " + msg +" | ~/mobileInsight/automator/ws_dissector/ws_dissector ")
    # FIXME: to be replaced with real message type ID
    return "LTE_RRC_OTA_Packet",output

def test_feed(filename):
    f = open(filename, "rb")
    buf = f.read()
    msg_list = feed_binary(buf)
    f.close()
    return msg_list

def seek_pstrace_magic(bytes):

    # idx_header = find_subtrace(header, bytes)
    # if idx_header:
    #     bytes = bytes[:idx_header] + bytes[idx_header+6:]
    # global a
    msg_id = bytes[0:4]
    # a += 1
    # print a, len(bytes), msg_id #, '\nbytes:\n',bytes[:50]

    # if a > 24:
    #     return []
    pstrace = []
    if msg_id in global_msg_id:

        # calculate the length of raw_data
        decimal_high = int(bytes[4], 16)
        decimal_low = int(bytes[5], 16)
        length = decimal_low * 256 + decimal_high
        # print 'hello', length, '\n\n', bytes[:20]
        if length > 0 and length < len(bytes):
            raw_bytes = bytes[6:6 + length]
            raw_data = map(lambda x: x if (x != '0x0') else '0x00', raw_bytes)
            raw_msg = ['0x00'] * 3 + [hex(global_ws_id[global_msg_id.index(msg_id)])] + ['0x00'] * 2 + [
                bytes[5]] + [bytes[4]] + raw_data
            pstrace.append(raw_msg)
            # print 'added', length, msg_id
            # print "\\" + "\\".join([j[1:] for j in raw_bytes])
            return pstrace
    # print "None====================="
    return pstrace

def parse_muxraw_magic(filename):
    msg_list = []
    # global a
    with open(filename, 'rb') as f:
        f.seek(0)
        byte = f.read(1)
        bytes_current = [] # store bytes in current section

        # parse file into sections devided by '\0x8f\0x9a\0x9a\0x8d\0x04\0x00'
        header = ['0xac', '0xca', '0x0', '0xff']
        header_magic = ['0x8f', '0x9a', '0x9a', '0x8d', '0x4', '0x0']
        while len(byte) ==1 and byte != 'ELF':
            # if a > 25:
            #     return msg_list
            byte_value = struct.unpack('B', byte)[0] # byte_value couldn't be print out
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
    raw_msg_list = test_feed(filename)
    msg_list = []
    # print len(raw_msg_list)

    if raw_msg_list != []:
        for i in raw_msg_list:
            output =  "\\" + "\\".join([j[1:] for j in i[0]])
            msg_list.append(output)
            # print 'msg'
            # print i[0]
            # print output
    # print msg_list
    if raw_msg_list != []:
        for msg in raw_msg_list:
            print decode(msg)
