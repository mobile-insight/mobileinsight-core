#!/usr/bin/python
# Filename: utils_android.py
"""
Utility functions for android

Author: Yuanjie Li
"""

from enum import Enum

class ChipsetType(Enum):

    QUALCOMM = 0
    MTK = 1

def run_shell_cmd(cmd, wait=False):
    p = subprocess.Popen(
        "su",
        executable=ANDROID_SHELL,
        shell=True,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE)
    res, err = p.communicate(cmd + '\n')

    if wait:
        p.wait()
        return res
    else:
        return res

def get_chipset_type():
    """
    Determine the type of the chipset

    :returns: an enum of ChipsetType
    """


    """
    MediaTek: [ro.board.platform]: [mt6735m]
    Qualcomm: [ro.board.platform]: [msm8084]
    """
    cmd = "getprop ro.board.platform;"
    res = run_shell_cmd(cmd).split('\n')
    if res.startswith("mt"):
        return ChipsetType.MTK
    elif res.startswith("msm") or res.startswith("mdm"):
        return ChipsetType.QUALCOMM
    else:
        return None