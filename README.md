rotinom-trop-moc
================

This tool is for phones based on the Qualcomm chip set.
The phone requires a SIM card.

First, configure the phone. (Instructions shown for Samsung Galaxy S4 and S5)

1) Install the driver for the phone.

2) In the keypad, type *#8080# to show the USBSettings. In the USBSettings, select AP and RNDIS + DM + MODEM.

3) Install Python v2.7 (32 bit).
   Also, install the following python libraries:
		pyserial (v2.7)
		crcmod (v1.7)

4) To run automator.py, run "python automator.py -p *insert_COM_port_here*"
   The automator will disable logs from the phone and then send commands in a given file.

   By default, automator.py will look for the file "example.foobar" in the command_files directory and send the binary commands in the file to the phone. There is only 1 binary command in "example.foobar", which is to enable RRC-OTA messages.

   You can specify a custom commands file to automator.py by using the -c flag.

   The messages sent from the phone will print onto the console. To save the messages to a file, use the -l flag.
   Example: python automator.py -p COM4 -c ../command_files/commands.txt -l ../test_logs.txt
