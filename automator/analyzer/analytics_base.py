#! /usr/bin/env python
"""
analytics_base.py

An abstraction for advanced analytics apps

Author: Yuanjie Li
"""

"""
Three levels of analytics modules
	- Raw-trace based module: e.g., RRC config
	- Advanced module: e.g., Bayesian analytics
		- This can be viewed as some applications
	- Applications: they can call both raw-trace module and advanced module
	
"""

#TODO: how to guarantee the synchronization between modules???
class AnalyticsModule:


"""
	Design goal
		- Event-driven programming over given analytics
			- The event can be deferred for evaluation
		- The event can be defined over multiple modules (e.g., RRC.SIB>th1 and ML1.rsrp>th2)
	Design overview
		- Event should be abstracted as a boolean function
		- We should only offer
			- The event-driven programming API
			- The event queue
			- The medium analytics model
"""

class AnalyticsApp:

