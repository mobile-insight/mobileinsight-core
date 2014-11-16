import crcmod
import crcmod.predefined
from binascii import unhexlify

s = unhexlify('5c0100c20100')

crc16 = crcmod.predefined.Crc('x-25')
crc16.update(s)
print crc16.hexdigest() # => 0x2ba4
