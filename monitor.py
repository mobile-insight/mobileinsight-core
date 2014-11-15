# -*- coding: utf-8 -*-
"""
monitor.py
COM port monitor. Work with SAMSUNG phone and com0com, a Null-modem emulator.
To learn about com0com, see http://com0com.sourceforge.net/ .

Author: Jiayao Li, Samson Richard Wong
"""

import sys
import time
import string
import serial
import serial.tools.list_ports
import optparse


def init_opt():
    """
    Initialize and return the option parser.
    """
    opt = optparse.OptionParser(prog="com-monitor",
                                usage="usage: %prog [options] LOGFILE ...",
                                description="COM serial port sniffer.")
    opt.add_option("-p", "--phy-serial-name",
                    metavar="STR",
                    action="store", type="string", dest="phy_serial_name", 
                    help="Manually set the name of physical serial name.")
    opt.add_option("-v", "--vir-serial-name",
                    metavar="STR",
                    action="store", type="string", dest="vir_serial_name", 
                    help="Manually set the name of virtual serial name.")
    opt.add_option("--phy-baudrate",
                    metavar="N",
                    action="store", type="int", dest="phy_baudrate", 
                    help="Set the physical baud rate [default: %default].")
    opt.add_option("--log-comment",
                    metavar="STR",
                    action="store", type="string", dest="log_comment", 
                    help="Add a beginning comment on log.")
    opt.set_defaults(phy_baudrate=9600)
    return opt


def static_var(varname, value):
    def decorate(func):
        setattr(func, varname, value)
        return func
    return decorate


def str_to_hex(s):
    """
    Convert each character of a string to its hex value, then return a
    whitespace-separated string of hex values.
    """
    return " ".join(c.encode("hex") for c in s)


@static_var("printset", set(string.printable))
def str_to_printable(s):
    return "".join(c if c in str_to_printable.printset else "." for c in s)


def detect_ports():
    """
    Automatically detect and return the physical and virtual COM port name.
    """
    ports = serial.tools.list_ports.comports()
    phy_ser_name = None
    vir_ser_name = None
    for name, description, hardware_id in ports:
        # Find the first port name of com0com port pair (CNCA1, CNCB1)
        if description.startswith("com0com - serial port emulator CNCA1"):
            vir_ser_name = name
        # Find the physical name for SAMSUNG phone
        # WARNING(Jiayao): in usual cases there will be more than one port found.
        # On my computer, the lowest one is what we want. This may vary from 
        # computer to computer.
        elif description.startswith("SAMSUNG Mobile USB Serial Port"):
            if phy_ser_name is None or phy_ser_name > name:
                phy_ser_name = name
    return phy_ser_name, vir_ser_name


def print_stats(income_KBps, output_KBps):
    print "Income: %.3f KBps; Output: %.3f KBps" % (income_KBps, output_KBps)
    return


if __name__ == "__main__":
    opt = init_opt()
    options, args = opt.parse_args(sys.argv[1:])

    if len(args) == 0:      # args[0] is NOT the name of script
        opt.print_help()
        sys.exit(0)

    log_name = args[0]
    print "LOG FILE: %s" % log_name

    if options.phy_serial_name is None or options.vir_serial_name is None:
        phy_ser_name, vir_ser_name = detect_ports()
    if options.phy_serial_name:
        phy_ser_name = options.phy_serial_name
    if options.vir_serial_name:
        vir_ser_name = options.vir_serial_name
    print "PHY COM: %s" % phy_ser_name
    print "VIR COM: %s" % vir_ser_name

    if phy_ser_name is None or vir_ser_name is None:
        sys.stderr.write("Serial port name error.\n")
        sys.exit(1)

    phy_baudrate = options.phy_baudrate
    print "PHY BAUD RATE: %d" % phy_baudrate

    log_comment = options.log_comment
    if log_comment:
        print "LOG COMMENT: %s" % log_comment

    try:
        log = open(log_name, "w")
        if log_comment:
            log.write("#%s\n" % log_comment)

        start = time.time()
        call_period = 2         # call print_stats every 2 secs
        last_call = start
        income_bytes = 0        # data bytes read from physical port during the period
        output_bytes = 0        # data bytes written to physical port during the period

        # Open COM ports. A zero timeout means that IO functions never suspend.
        vir_ser = serial.Serial(vir_ser_name, timeout=0)
        phy_ser = serial.Serial(phy_ser_name, baudrate=phy_baudrate, timeout=0)

        while True:
            s = vir_ser.read(32)
            now = time.time()
            if s:
                log.write("<%.4f OUT %d \n%s\n" % (now - start, len(s), str_to_hex(s)))
                output_bytes = output_bytes + len(s)
                phy_ser.write(s)

            s = phy_ser.read(64)
            now = time.time()
            if s:
                log.write(">%.4f IN %d \n%s\n" % (now - start, len(s), str_to_hex(s)))
                income_bytes = income_bytes + len(s)
                vir_ser.write(s)

            # call print_stats every 2 secs
            now = time.time()
            if now - last_call > call_period:
                print_stats(income_bytes / (1000. * call_period), output_bytes / (1000. * call_period))
                income_bytes = output_bytes = 0
                last_call = now

    except IOError, e:
        sys.exit(e)
    except Exception,e:
        log.close()
        sys.exit(e)
    else:
        log.close()
