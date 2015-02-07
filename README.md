automator
=========

# Introduction #

This tool is designed to collect runtime traces from mobile phones using Qualcomm chipset.

![Module structure](docs/modules.png)
  
  * Serial port communication: communicates with COM port using pySerial library.
  * HDLC-like frame decoder: en/decode the raw bytes read from the COM port. Written in native Python code.
  * QCDM protocol endec: (Not implemented yet) en/decode QCDM commands and messages.
  * Automator: the main program.
  * Target trace configuration: a set of configuration files that describe what types of messages users care about.

# Distribution #

This tool can be run in two forms: Python script(.py file) and Windows executable form(.exe file). The second form is generated based on the first one, so the functionality is identical.
However, the executable form is more convenient for distribution purpose, because it is a stand-alone program and does not require any prerequisite application or library.

Before generating Windows executable file, please ensure that all prerequisites are installed, then use the following command line:

    python setup.py py2exe

then you will find the automator.exe in the dist/ directory, along with some default configuration files under dist/command_files/ directory.

# Usage #

## Before running ##

Make sure that your phone is based on the Qualcomm chip set. The phone also requires a SIM card.

(1) Install the driver for the phone.

(2) Configure the phone. 

  * For Samsung Galaxy S4 and S5, type *#8080# in the keypad to show the USBSettings menu, then select AP and RNDIS + DM + MODEM mode (or any other option that includes both "DM" and "MODEM" words).
  * For other models, we have not tested yet.

(3) Connect the phone with PC using USB cable. You will need to find out which COM port your phone is using to run the tool. Depending on your operating system, there are several ways to do so

  * On Windows, you can open Windows device manager and check the hardware list. If you succeed in step (2), you will find a modem called "Qualcomm modem", "Samsung Android Modem" or any similar name under "Modems" catagory. There will also be several COM ports showing up in the "Ports (COM & LPT)" catagory. One of them will be the correct port, so you have to try.
  * The pySerial library has also provide a small utility to list all available serial ports. See pySerial documentation for more information.

(4) (Skip this step if you are using automator.exe) Install Python v2.7 (32 bit) and the following python libraries:

    pyserial (v2.7)
    crcmod (v1.7)

## Get started ##

You can run the tool using automator.exe (given in the distribution version) or with the actual python script. We only present command lines for .exe version, but all flags and options described are appliable to .py version.

To start automator, run

    automator -p *insert_COM_port_here*
    
Automator will try to connect to the serial port first. If any other program (e.g. QPST) is using this port, the connection will fail.

After successful conenction, automator will reset logging of the phone and then send commands to re-enable logging messages. Only messages of desired types will be received, and which type to be desired is user-configurable. By default, an example configuration is used. Use -c flag to replace it with custom configuration.

The messages received from the phone will print onto the console. To save the messages to a file as well, use the -l flag.

Example (for .exe):

    automator -p COM4 -c ./command_files/commands.txt -l ../test_logs.txt

For description of all the options, use the -h or --help flags.

## Configuration files ##

All configuration files are located in the ./command_files directory (for exe) or the ../command_files directory (for the Python script). Two configuration files are used: *example_cmds.txt* and *cmd_dict.txt*.

(Following paragraphs are subject to change)

You can specify a custom commands file to automator by using the -c flag or you may edit the example_cmds.txt file, which is used by default. There is only 1 command in "example_cmds.txt", which is to enable RRC-OTA messages. 

The command is in human readable form, and the mapping from the human readable text to the binary code are contained in cmd_dict.txt (located in the same directory as example_cmds.txt).
Feel free to add your own commands there. Please note that in the commands given, enabling one type of log and then attempting to enable another type of log will disable the first log. For example, sending the RRC-OTA command then sending the RRC-MIB command will disable the RRC-OTA logs. You will have to create your own binary to send to the phone to enable multiple logs at the same time.
