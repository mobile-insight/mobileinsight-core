automator
=========

# Introduction #

This tool is designed to collect runtime traces from mobile phones using Qualcomm chipset.

![alt tag](docs/modules.png)

# Distribution #

This tool can be used in two forms: Python script(.py file) and Windows executable form(.exe file). The second form is generated based on the first one, so the functionality is identical.
However, the executable form is more convenient for distribution purpose, because it is a stand-alone program and does not require any prerequisite application or library.

To generate Windows executable file, use the following command line:

    python setup.py py2exe

then you will find the automator.exe in the dist/ directory, along with some default configuration files under dist/command_files/ directory.

# Usage #

This tool is for phones based on the Qualcomm chip set. The phone requires a SIM card.

First, configure the phone. (Instructions shown for Samsung Galaxy S4 and S5)

1) Install the driver for the phone.

2) In the keypad, type *#8080# to show the USBSettings. In the USBSettings, select AP and RNDIS + DM + MODEM.

3) (Skip if you are using the automator.exe, this is only required for using automator.py) Install Python v2.7 (32 bit) and the following python libraries:

    pyserial (v2.7)
    crcmod (v1.7)

4) You can run the tool using automator.exe (given in the distribution) or with the actual python script. You will need to find out which COM port your phone is using to run the tool.

To use the exe, run 

    automator -p *insert_COM_port_here*
    
To use the script(automator.py), run:

    python automator.py -p *insert_COM_port_here*


The automator will disable logs from the phone and then send commands that are specified in a given file. You can specify a custom commands file to automator.py by using the -c flag or you may edit the example_cmds.txt file, which is used by default and is located in the ./command_files directory (for exe) or the ../command_files directory (for the python script).

There is only 1 command in "example_cmds.txt", which is to enable RRC-OTA messages. 
The command is in human readable form, and the mapping from the human readable text to the binary code are contained in cmd_dict.txt (located in the same directory as example_cmds.txt).
Feel free to add your own commands there. Please note that in the commands given, enabling one type of log and then attempting to enable another type of log will disable the first log. For example, sending the RRC-OTA command then sending the RRC-MIB command will disable the RRC-OTA logs. You will have to create your own binary to send to the phone to enable multiple logs at the same time.

The messages sent from the phone will print onto the console. To save the messages to a file as well, use the -l flag.

Example (for the exe):

    automator -p COM4 -c ./command_files/commands.txt -l ../test_logs.txt

Example (for the script):

    python automator.py -p COM4 -c ../command_files/commands.txt -l ../test_logs.txt

For descriptions of all the options, use the -h or --help flags.
