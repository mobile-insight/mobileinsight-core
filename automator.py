from sender import sendMessage
from sender import recvMessage
from hdlc_parser import hdlc_parser
import optparse
import sys
import serial

def init_opt():
    """
    Initialize and return the option parser.
    """
    opt = optparse.OptionParser(prog="com-sender",
                                usage="usage: %prog [options] LOGFILE ...",
                                description="COM serial port sender.")
    opt.add_option("-p", "--phy-serial-name",
                    metavar="STR",
                    action="store", type="string", dest="phy_serial_name", 
                    help="Manually set the name of physical serial name.")
    opt.add_option("-l", "--log-name",
                    metavar="STR",
                    action="store", type="string", dest="log_name", 
                    help="Specify a log file to save packets in")
    opt.add_option("--phy-baudrate",
                    metavar="N",
                    action="store", type="int", dest="phy_baudrate", 
                    help="Set the physical baud rate [default: %default].")
    opt.set_defaults(phy_baudrate=9600)
    return opt

if __name__ == "__main__":
    opt = init_opt()
    options, args = opt.parse_args(sys.argv[1:])

    if options.phy_serial_name:
        phy_ser_name = options.phy_serial_name
    print "PHY COM: %s" % phy_ser_name

    log = None
    if options.log_name is not None:
        log = open(options.log_name, "w")

    if phy_ser_name is None:
        sys.stderr.write("Serial port name error.\n")
        sys.exit(1)

    phy_baudrate = options.phy_baudrate
    print "PHY BAUD RATE: %d" % phy_baudrate

    try:
        # Open COM ports. A zero timeout means that IO functions never suspend.
        phy_ser = serial.Serial(phy_ser_name, baudrate=phy_baudrate, timeout=.5)
        parser = hdlc_parser()

        # disable logs
        s = "73 00 00 00 00 00 00 00"
        sendMessage(phy_ser, s)
        print recvMessage(parser, phy_ser, s[0:2]) + "\n"

        # start rrc-ota messages
        s = "73 00 00 00 03 00 00 00 0B 00 00 00 09 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
        sendMessage(phy_ser, s)
        print recvMessage(parser, phy_ser, s[0:2]) + "\n"

        while True:
            #cmd = 10 for log packets
            rec = recvMessage(parser, phy_ser, "10") 
            if rec != "":
                print rec + "\n"
                if log is not None:
                    log.write(rec + "\n\n")
    except KeyboardInterrupt, e:
        # disable logs
        s = "73 00 00 00 00 00 00 00"
        sendMessage(phy_ser, s)
        print recvMessage(parser, phy_ser, s[0:2]) + "\n"
        sys.exit(e)
    except IOError, e:
        sys.exit(e)
    except Exception,e:
        sys.exit(e)