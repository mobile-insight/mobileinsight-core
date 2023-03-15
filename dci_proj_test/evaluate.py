import ast
import datetime
from deepdiff import DeepDiff
from pprint import pprint

"""
Test the decoder by comparing all messages with the corresponding ground truth.
Note that the input test.txt file should be in DICT format.
"""

# helper function for get_msg_gt. read dci according to format
def read_dci(fields):
    dci = {}
    dci['Carrier ID'] = int(fields[0])
    dci['RNTI Type'] = fields[1]
    dci['DCI Format'] = fields[2]
    dci['Aggregation Level'] = fields[3]
    dci['Raw DCI Included'] = int(fields[4])
    dci['DCI Params'] = {}
    if dci['DCI Format'] == 'DL_1_0':
        dci['DCI Params']['DL'] = {}
        dci['DCI Params']['DL']['Bandwidth Part Indicator'] = int(fields[5])
        dci['DCI Params']['DL']['Time Resource Assignment'] = int(fields[6])
        dci['DCI Params']['DL']['TB 1 MCS'] = int(fields[7])
        dci['DCI Params']['DL']['TB 1 New Data Indicator'] = int(fields[8])
        dci['DCI Params']['DL']['DL Assignment Index'] = int(fields[9])
        dci['DCI Params']['DL']['TPC Command For Sched PUCCH'] = int(fields[10])
        dci['DCI Params']['DL']['PUCCH Resource Indicator'] = int(fields[11])
        dci['DCI Params']['DL']['PDSCH Harq Feedback Timing'] = int(fields[12])
        dci['DCI Params']['DL']['CBG Transmission Info'] = int(fields[13])
        dci['DCI Params']['DL']['CBG Flushing Out Info'] = int(fields[14])
        dci['DCI Params']['DL']['Transmission Config Ind'] = int(fields[15])
        dci['DCI Params']['DL']['SRS Request'] = int(fields[16])
        dci['DCI Params']['DL']['Carrier ID'] = int(fields[17])
        dci['DCI Params']['DL']['HARQ ID'] = int(fields[18])
    elif dci['DCI Format'] == 'UL_0_1':
        dci['Raw DCI[2]'] = fields[5]
        dci['Raw DCI[1]'] = fields[6]
        dci['Raw DCI[0]'] = fields[7]
        dci['Raw DCI Payload'] = {}
        dci['Raw DCI Payload']['Raw DCI Payload[2]'] = int(dci['Raw DCI[2]'], base=16)
        dci['Raw DCI Payload']['Raw DCI Payload[1]'] = int(dci['Raw DCI[1]'], base=16)
        dci['Raw DCI Payload']['Raw DCI Payload[0]'] = int(dci['Raw DCI[0]'], base=16)
        dci['DCI Params']['UL'] = {}
        dci['DCI Params']['UL']['DCI Format'] = dci['DCI Format']
        dci['DCI Params']['UL']['Carrier ID'] = int(fields[8])
        dci['DCI Params']['UL']['NDI'] = int(fields[9])
        dci['DCI Params']['UL']['MCS'] = int(fields[10])
        dci['DCI Params']['UL']['Freq Hopping Flag'] = int(fields[11])
        dci['DCI Params']['UL']['RV'] = int(fields[12])
        dci['DCI Params']['UL']['HARQ ID'] = int(fields[13])
        dci['DCI Params']['UL']['PUSCH TPC'] = int(fields[14])
        dci['DCI Params']['UL']['UL SUL Ind'] = int(fields[15])
        dci['DCI Params']['UL']['Symbol Alloc Index'] = int(fields[16])
        dci['DCI Params']['UL']['BWP Ind'] = int(fields[17])
        dci['DCI Params']['UL']['PTRS DMRS Association'] = int(fields[18])
        dci['DCI Params']['UL']['Beta Offset Ind'] = int(fields[19])
        dci['DCI Params']['UL']['RB Assignment'] = int(fields[20])
        dci['DCI Params']['UL']['UL SCH Ind'] = int(fields[21])
        dci['DCI Params']['UL']['DAI 1'] = int(fields[22])
        dci['DCI Params']['UL']['DAI 2'] = int(fields[23])
        dci['DCI Params']['UL']['SRS Resource Indication'] = int(fields[24])
        dci['DCI Params']['UL']['Precoding Layer Info'] = int(fields[25])
        dci['DCI Params']['UL']['Antenna Ports'] = int(fields[26])
        dci['DCI Params']['UL']['DMRS Seq Init Flag'] = int(fields[27])
        dci['DCI Params']['UL']['SRS Request'] = int(fields[28])
        dci['DCI Params']['UL']['CSI Request'] = int(fields[29])
        dci['DCI Params']['UL']['CBGTI'] = int(fields[30])
        dci['DCI Params']['UL']['Reserved MCE Enable'] = int(fields[31])
        dci['DCI Params']['UL']['RA Type'] = int(fields[32])
        dci['DCI Params']['UL']['Prune Reason'] = fields[33]
        dci['DCI Params']['UL']['Pruned Mask'] = fields[34]
    elif dci['DCI Format'] == 'DL_1_1':
        dci['DCI Params']['DL'] = {}
        dci['DCI Params']['DL']['Bandwidth Part Indicator'] = int(fields[5])
        dci['DCI Params']['DL']['Time Resource Assignment'] = int(fields[6])
        dci['DCI Params']['DL']['TB 1 MCS'] = int(fields[7])
        dci['DCI Params']['DL']['TB 1 New Data Indicator'] = int(fields[8])
        dci['DCI Params']['DL']['DL Assignment Index'] = int(fields[9])
        dci['DCI Params']['DL']['TPC Command For Sched PUCCH'] = int(fields[10])
        dci['DCI Params']['DL']['PUCCH Resource Indicator'] = int(fields[11])
        dci['DCI Params']['DL']['PDSCH Harq Feedback Timing'] = int(fields[12])
        dci['DCI Params']['DL']['CBG Transmission Info'] = int(fields[13])
        dci['DCI Params']['DL']['CBG Flushing Out Info'] = int(fields[14])
        dci['DCI Params']['DL']['Transmission Config Ind'] = int(fields[15])
        dci['DCI Params']['DL']['SRS Request'] = int(fields[16])
        dci['DCI Params']['DL']['Carrier ID'] = int(fields[17])
        dci['DCI Params']['DL']['HARQ ID'] = int(fields[18])
    return dci

# get the groud truth from the example logs
def get_msg_gt():
    f_groundtruth = open("diag_log_20221101_141847_68b329da9893e34099c7d8ad5cb9c940_Google-Pixel5_.txt", "r")
    groundtruth = f_groundtruth.readlines()
    f_groundtruth.close()
    msg_gt = []
    for i in range(len(groundtruth)):
        if "Major.Minor Version" in groundtruth[i]:
            m = {}
            m['MacVersion'] = {}
            tmp1 = [x.strip() for x in groundtruth[i].split()]
            m['MacVersion']['Major Version'] = int(tmp1[-2][:-1])
            m['MacVersion']['Minor Version'] = int(tmp1[-1])
            m['MacVersion']['Major.Minor Version'] = m['MacVersion']['Major Version']
            m['MacVersion']['{id: 2026230 }'] = m['MacVersion']['Minor Version']
            tmp2 = [x.strip() for x in groundtruth[i+1].split()]
            m['Version 131077'] = {}
            m['Version 131077']['Log Fields Change BMask'] = int(tmp2[-1])
            tmp3 = [x.strip() for x in groundtruth[i+2].split()]
            m['Version 131077']['Num Records'] = int(tmp3[-1])
            m['Records'] = []
            idx = 0
            for j in range(m['Version 131077']['Num Records']):
                record = {}
                fields = [x.strip() for x in groundtruth[i+13+idx].split('|') if x.strip() != '']
                idx += 1
                record['System Time'] = {}
                record['System Time']['Slot'] = int(fields[1])
                record['System Time']['Num'] = fields[2]
                record['System Time']['Frame'] = int(fields[3])
                record['Num DCI'] = int(fields[4])
                record['DCI Info'] = [read_dci(fields[5:])]
                if record['Num DCI'] > 0:
                    for k in range(record['Num DCI']-1):
                        record['DCI Info'].append(read_dci([x.strip() for x in groundtruth[i+13+idx].split('|') if x.strip() != '']))
                        idx += 1
                m['Records'].append(record)
            tmp4 = [x.strip() for x in groundtruth[i+13+idx+3].split()]
            m['log_msg_len'] = int(tmp4[-1])
            msg_gt.append(m)
    return msg_gt


# ----------------------------------------------------------------------------------------------------------------------------------------

f = open("decoded_dci.txt", "r")

# ground truth
msg_gt = get_msg_gt()

all_match = True

# compare all messages
for i in range(len(msg_gt)):
    gt = msg_gt[i]
    line = f.readline()
    msg = eval(line)
    # skip the packets that are not NR_DCI_Message
    while msg['type_id'] != 'NR_DCI_Message':
        line = f.readline()
        msg = eval(line)
    # remove fields we don't care about
    msg.pop('type_id', None)
    msg.pop('timestamp', None)
    msg['Version 131077'].pop('Log Fields Change', None)
    comparison = DeepDiff(gt, msg)
    if comparison != {}:
        all_match = False
        print("MSG", i, "differs:")
        pprint(comparison, indent=2)

f.close()

if all_match:
    print("All {} decoded messages match the ground truth!".format(len(msg_gt)))