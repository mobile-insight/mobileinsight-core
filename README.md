MobileInsight
==============

MobileInsight is a Python (2.7) library for mobile network monitoring and analysis on the end device. It is developed by UCLA Wireless Networking Group (Wing), and licensed under Apache Licence 2.0.

It provides low-level monitors for mobile network (3G/4G), and extensible event-driven analyzers, such as those for radio resource control (RRC), mobility management (EMM), and session management (ESM). 
You can also define your own analyzers for customized usage.

Typical use involves declaring a monitor to track the network status, and ananlyzers for online/offline analysis. See [overview](http://metro.cs.ucla.edu/mobile_insight/overview.html) on how this works, and `[examples](http://metro.cs.ucla.edu/mobile_insight/examples.html) on how to use it.

![Module structure](docs/overview.png)