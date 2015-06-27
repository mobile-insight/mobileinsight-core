MobileInsight
==============

# Introduction #

MobileInsight is a Python (2.7) library for mobile network monitoring and analysis on the end device. It is developed by UCLA Wireless Networking Group (Wing), and licensed under Apache Licence 2.0.

It provides low-level monitors for mobile network (3G/4G), and extensible event-driven analyzers, such as those for radio resource control (RRC), mobility management (EMM), and session management (ESM). 
You can also define your own analyzers for customized usage.

Typical use involves declaring a monitor to track the network status, and ananlyzers for online/offline analysis. See [overview](http://metro.cs.ucla.edu/mobile_insight/overview.html) on how this works, and [examples](http://metro.cs.ucla.edu/mobile_insight/examples.html) on how to use it.

![Module structure](docs/overview.png)


# Installation #

Please see [here](http://metro.cs.ucla.edu/mobile_insight/installation.html) for more detailed instructions. The following is a quick summary of installation:

First install [Python 2.7](https://www.python.org/) if you do not have them. 

Second, MobileInsight builds on top of pyserial and crcmod, so please install both libraries:

    pip install pyserial
    pip install crcmod

If you need visualization for some results (e.g., plotting the signal strength), 
please install matplotlib:

    pip install matplotlib


Third, MobileInsight relies on Wireshark to decode the 3G/4G messages. Please install [Wireshark](https://www.wireshark.org/download.html) with either binary executables or source code. 

Last, download MobileInsight [here](http://metro.cs.ucla.edu/mobile_insight//download.html), and unpack the code::

    tar -zxvf MobileInsight-1.0.tar.gz
    cd MobileInsight-1.0
    python setup.py install

