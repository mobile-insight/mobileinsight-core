#! /usr/bin/env python
"""
lte_ue_analyzer.py

An analyzer for LTE user's internal events

Author: Yuanjie Li
"""

from analyzer import *

import pylab
from pylab import *
from multiprocessing import Process,Pipe

class LteUeAnalyzer(Analyzer):

    def __init__(self):

        Analyzer.__init__(self)

        #init packet filters
        self.add_source_callback(self.ue_event_filter)

        self.serv_cell_rsrp=[-140 for x in range(100)]

        self.plot_conn,recv_conn=Pipe()
        self.plot_p=Process(target=show_serv_rsrp,args=(recv_conn,))
        self.plot_p.start()

    def __del__(self):
        print "kill %s" % self.plot_p
        if self.plot_p.is_alive():
            self.plot_conn.close()
            self.plot_p.kill()

    def set_source(self,source):
        Analyzer.set_source(self,source)
        #enable user's internal events
        source.enable_log("LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results")

    def ue_event_filter(self,msg):
        #TODO: support more user events
        self.serving_cell_rsrp(msg)

    def serving_cell_rsrp(self,msg):
        if msg.type_id == "LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results":
            msg_dict=dict(msg.data)
            print msg.timestamp,msg_dict['Serving Filtered RSRP(dBm)']
            self.serv_cell_rsrp.append(msg_dict['Serving Filtered RSRP(dBm)'])
            self.plot_conn.send(self.serv_cell_rsrp)


def show_serv_rsrp(conn):
    xAchse=pylab.arange(0,100,1)
    yAchse=pylab.array([0]*100)

    fig = pylab.figure(1)
    ax = fig.add_subplot(111)
    ax.grid(True)
    ax.set_title("Serving Cell Measurements")
    ax.set_xlabel("Time")
    ax.set_ylabel("RSRP (dBm)")
    ax.axis([0,100,-140,-60])
    line=ax.plot(xAchse,yAchse,'-')

    manager = pylab.get_current_fig_manager()

    timer = fig.canvas.new_timer(interval=20)
    timer.add_callback(RealtimePloter,conn,ax,line,manager)

    timer.start()
    pylab.show()


def RealtimePloter(conn,ax,line,manager):
    values = conn.recv()
    CurrentXAxis=pylab.arange(len(values)-100,len(values),1)
    line[0].set_data(CurrentXAxis,pylab.array(values[-100:]))
    ax.axis([CurrentXAxis.min(),CurrentXAxis.max(),-140,-60])
    manager.canvas.draw()

