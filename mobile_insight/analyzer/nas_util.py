#!/usr/bin/python
# Filename: nas_util.py

"""

Some utility functions for 3G/4G NAS

Author: Yuanjie Li
"""


# QoS mapping: 10.5.6.5, TS24.008
mean_tput = {
    0: "subscribed throughput",
    1: 100, 2: 200, 3: 500, 4: 1000, 5: 2000, 6: 5000, 7: 10000, 8: 20000,
    9: 50000, 10: 100000, 11: 200000, 12: 500000, 13: 1000000, 14: 2000000,
    15: 5000000, 16: 10000000, 17: 20000000, 18: 50000000, 31: "best effort"}


delivery_order = {1: "with delivery order", 2: "without delivery order"}

bearer_type = {0: "default", 1: "dedicated"}


traffic_class = {1: "conversional class", 2: "streaming class",
                 3: "interactive class", 4: "background class"}


residual_ber = {
    0: "subscribed residual bit error rate",
    1: 5e-2, 2: 1e-2, 3: 5e-3, 4: 4e-3, 5: 1e-3, 6: 1e-4, 7: 1e-5,
    8: 1e-6, 9: 6e-8}


def xstr(val):
    '''
    Return a string for valid value, or empty string for Nontype

    :param val: a value
    :returns: a string if val is not none, otherwise an empty string
    '''

    if val:
        return str(val)
    else:
        return "unknown"


def max_bitrate(val):
    '''
    Given ESM value, return maximum bit rate (Kbps).
    Please refer to 10.5.6.5, TS24.008 for more details.

    :param val: the value encoded in the ESM NAS message
    '''
    if val <= 63:
        return val
    elif val <= 127:
        return 64 + (val - 64) * 8
    elif val <= 254:
        return 576 + (val - 128) * 64
    else:
        return 0


def max_bitrate_ext(val):
    """
    Given ESM value, return extended maximum bit rate (Kbps).
    Please refer to 10.5.6.5, TS24.008 for more details.

    :param val: the value encoded in the ESM NAS message
    """
    if val <= 74:
        return 8600 + val * 100
    elif val <= 186:
        return 16000 + (val - 74) * 1000
    elif val <= 250:
        return 128000 + (val - 186) * 2000
    else:
        return None


def trans_delay(val):
    """
    Given ESM value, return transfer delay (ms).
    Please refer to 10.5.6.5, TS24.008 for more details.

    :param val: the value encoded in the ESM NAS message
    """
    if val <= 15:
        return val * 10
    elif val <= 31:
        return 200 + (val - 16) * 50
    elif val <= 62:
        return 1000 + (val - 32) * 100
    else:
        return None
