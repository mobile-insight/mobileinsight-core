#!/usr/bin/python
# Filename: online_monitor.py
"""
A universal, cross-platform MobileInsight online monitor.

It abstracts the low-level complexity of platform-dependent monitors.
It wraps monitors for mobile version (currently Android only) and desktop version.

Author: Yuanjie Li
"""

__all__ = ["OnlineMonitor"]

# Test the OS version
is_android = False

try:
    from jnius import autoclass  # For Android

    is_android = True

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
        elif res.startswith(b"msm") or res.startswith(b"mdm") or res.startswith(b"sdm") or res.startswith(b"kona") or res.startswith(b"lito"):
            return ChipsetType.QUALCOMM
        else:
            print(("WARNING: Unknown type:", res))
            return None


    chipset_type = get_chipset_type()
    print("Chipset Type ID: ", chipset_type)

    if chipset_type == ChipsetType.QUALCOMM:
        from .android_dev_diag_monitor import AndroidDevDiagMonitor


        class OnlineMonitor(AndroidDevDiagMonitor):
            def __init__(self):
                AndroidDevDiagMonitor.__init__(self)

            def set_serial_port(self, phy_ser_name):
                """
                NOT USED: Compatability with DMCollector

                :param phy_ser_name: the serial port name (path)
                :type phy_ser_name: string
                """
                print("WARNING: Android version does not need to configure serial port")

            def set_baudrate(self, rate):
                """
                NOT USED: Compatability with DMCollector

                :param rate: the baudrate of the port
                :type rate: int
                """
                print("WARNING: Android version does not need to configure baudrate")
    elif chipset_type == ChipsetType.MTK:
        from .android_mtk_monitor import AndroidMtkMonitor


        class OnlineMonitor(AndroidMtkMonitor):
            def __init__(self):
                AndroidMtkMonitor.__init__(self)

            def set_serial_port(self, phy_ser_name):
                """
                NOT USED: Compatability with DMCollector

                :param phy_ser_name: the serial port name (path)
                :type phy_ser_name: string
                """
                print("WARNING: Android version does not need to configure serial port")

            def set_baudrate(self, rate):
                """
                NOT USED: Compatability with DMCollector

                :param rate: the baudrate of the port
                :type rate: int
                """
                print("WARNING: Android version does not need to configure baudrate")
    else:
        from .monitor import Monitor


        class OnlineMonitor(Monitor):
            def __init__(self):
                Monitor.__init__(self)
                self.log_warning("Unsupported chipset type")

            def set_serial_port(self, phy_ser_name):
                """
                NOT USED: Compatability with DMCollector

                :param phy_ser_name: the serial port name (path)
                :type phy_ser_name: string
                """
                print("WARNING: Android version does not need to configure serial port")

            def set_baudrate(self, rate):
                """
                NOT USED: Compatability with DMCollector

                :param rate: the baudrate of the port
                :type rate: int
                """
                print("WARNING: Android version does not need to configure baudrate")


except Exception as e:
    # import traceback
    # print str(traceback.format_exc())

    # not used, but bugs may exist on laptop
    from .dm_collector.dm_collector import DMCollector

    is_android = False


    class OnlineMonitor(DMCollector):
        def __init__(self):
            DMCollector.__init__(self)
