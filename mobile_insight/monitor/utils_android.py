#!/usr/bin/python
# Filename: utils_android.py
"""
Utility functions for android

Author: Yuanjie Li
"""

__all__ = ["ChipsetType","get_chipset_type","run_shell_cmd"]

import subprocess


ANDROID_SHELL = "/system/bin/sh"


class ChipsetType:
    """
    Cellular modem type
    """
    QUALCOMM = 0
    MTK = 1

def run_shell_cmd(cmd, wait=False):
    if isinstance(cmd, str):
        cmd = cmd.encode()
    p = subprocess.Popen(
        "su",
        executable=ANDROID_SHELL,
        shell=True,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE)
    res, err = p.communicate(cmd + b'\n')

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
    res = run_shell_cmd(cmd)
    if res.startswith(b"mt"):
        return ChipsetType.MTK
    elif res.startswith(b"msm") or res.startswith(b"mdm"):
        return ChipsetType.QUALCOMM
    else:
        return None