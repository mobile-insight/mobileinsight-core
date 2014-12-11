# -*- coding: utf-8 -*-
"""
hdlc_parser.py
Define hdlc_parser class that deals with HDLC frames in the raw logs produced by monitor.
If executed as a script, it translates a raw log into a new log, each log item of which
is a single frame.

Author: Jiayao Li
"""


__all__ = {"hdlc_parser"}


import struct
import crcmod
import binascii

class hdlc_parser:
    """
    This class takes fragmented HDLC encapsulated data and decodes its frame 
    structure. The clean HDLC payload is returned using Python iterator interface.
    """

    def __init__(self):
        self._remain_frames = []
        self._incomplete = None

    def feed_binary(self, ts, binary):
        """
        Feed the parser with a chunk of HDLC encapsulated data with timestamp.

        Returns:
            None

        Args:
            ts: the timestamp of this chunk of binary data.
            binary: a chunk of binary data.
        """
        b_lst = binary.split("\x7e")
        # print repr(b_lst)
        for i in range(len(b_lst)):
            b = b_lst[i]
            if i == len(b_lst) - 1:
                if not b:
                    continue
                # incomplete frame 
                if self._incomplete:
                    self._incomplete[1] += b
                else:
                    self._incomplete = [None, None]
                    self._incomplete[0] = ts
                    self._incomplete[1] = b
            else:
                if self._incomplete:
                    this_ts = self._incomplete[0]
                    this_fr = self._incomplete[1] + b
                    self._incomplete = None
                else:
                    this_ts = ts
                    this_fr = b
                if not this_fr:
                    continue
                payld, fcs = self._clean_escape(this_fr)
                crc_correct = self._check_crc(payld, fcs)
                self._remain_frames.append(tuple([this_ts, payld, fcs, crc_correct]))

    def _clean_escape(self, frame_binary):
        payld = []
        esc = False
        for c in frame_binary:
            if esc:
                orig_c = chr(ord(c) ^ 0x20)
                # print '7d %s ==> %s' % (binascii.b2a_hex(c), binascii.b2a_hex(orig_c))
                payld.append(orig_c)
                esc = False
            else:
                if c == '\x7d':
                    esc = True
                else:
                    payld.append(c)
        payld = ''.join(payld)
        fcs = struct.unpack("<H", payld[-2:])[0]   # unsigned short, little endian
        payld = payld[0:-2]
        return payld, fcs

    def _check_crc(self, payld, fcs):
        crc16 = crcmod.predefined.Crc('x-25')
        for c in payld:
            crc16.update(c)
        calc_crc = struct.unpack(">H", crc16.digest())[0]   # big endian
        # print 'Calc %s, FCS %s' % (hex(calc_crc), hex(fcs))
        return calc_crc == fcs

    def __iter__(self):
        """
        Returns an iterator over all parsed frames.

        For each parsed frame, a 4-element tuple (ts, payld, fcs, crc_correct)
        is returned. The meaning of each field is:
            ts: the timestamp of this frame
            payld: the payload part of this frame
            fcs: the integer value of the original Frame Checksum Sequence.
            crc_correct: whether this frame is correctly verified.
        """
        return self

    def next(self):
        if len(self._remain_frames) > 0:
            t, fr, fcs, crc_correct = self._remain_frames.pop(0)
            return t, fr, fcs, crc_correct
        else:
            raise StopIteration()


if __name__ == '__main__':
    import sys
    # b = "7d 5d 02 88 13 a5 13 c3 40 7e".replace(" ", "")
    # b += "7d 5d 02 7c 15 8c 15 c0 02 7e".replace(" ", "")
    # b = binascii.a2b_hex(b)
    # parser = hdlc_parser()
    # parser.feed_binary(0.0, b[0:5])
    # parser.feed_binary(0.5, b[5:7])
    # parser.feed_binary(1.0, b[7:])
    # for t, fr, fcs, crc_correct in parser:
    #     print t, binascii.b2a_hex(fr), hex(fcs), crc_correct
    # sys.exit(0)

    if len(sys.argv) != 3:
        print "Usage: hdlc_parser.py INPUT_LOG OUTPUT_LOG"
        sys.exit(0)
    log_name = sys.argv[1]
    out_name = sys.argv[2]

    in_parser = hdlc_parser()
    out_parser = hdlc_parser()
    with open(out_name, "w") as out:
        metadata = None
        for line in open(log_name):
            if line.startswith('<') or line.startswith('>'):
                ts, _, byt = line[1:].rstrip().split(None, 2)
                ts = float(ts)
                byt = int(byt)
                metadata = [line[0], ts, byt]
            elif line.startswith('#'):
                out.write(line)
            else:                
                if not metadata:
                    sys.stderr.write("Log Format Error 1.\n")
                    sys.exit(1)

                ts, byt = metadata[1:3]
                b = line.rstrip().replace(" ", "")
                b = binascii.a2b_hex(b)
                # print metadata
                if len(b) != byt:
                    sys.stderr.write("Log Format Error 2.\n")
                    sys.exit(1)
                parser = (out_parser if metadata[0] == '<' else in_parser)
                parser.feed_binary(ts, b)

                for t, fr, fcs, crc_correct in parser:
                    s1 = "OUT" if metadata[0] == '<' else "IN"
                    s2 = "Succeed" if crc_correct else "Corrupted"
                    out.write("%s%f %s %d\n" % (metadata[0], t, s1, len(fr)))
                    out.write("#FCS:%s %s\n" % (hex(fcs), s2))
                    out.write(" ".join(binascii.b2a_hex(c) for c in fr))
                    out.write("\n")
                metadata = None
