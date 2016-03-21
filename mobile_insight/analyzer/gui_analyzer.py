#!/usr/bin/python
# Filename: gui_analyzer.py
"""
A simple analyzer that displays the logs in a grid view

"""
import sys
import wx
import wx.grid

from analyzer import *

try: 
    import xml.etree.cElementTree as ET 
except ImportError: 
    import xml.etree.ElementTree as ET
import io
import datetime
from  mobile_insight.monitor.dm_collector.dm_endec.dm_log_packet import DMLogPacket

__all__=["GuiAnalyzer"]

class GuiAnalyzer(Analyzer):
    """
    A simple visualization of log events
    """
    
    def __init__(self):
        Analyzer.__init__(self)
        # a message dump has no analyzer in from/to_list
        # it only has a single callback for the source

        self.__msg_log=[] # in-memory message log
        self.add_source_callback(self.__dump_message)
        self.decode_type = 0

    def __dump_message(self,msg):
        """
        Print the received message

        :param msg: the received message
        """
        self.__msg_log.append(msg)
        # print msg.timestamp,msg.type_id
        #date = datetime.datetime.fromtimestamp(msg.timestamp).strftime('%Y-%m-%d %H:%M:%S.%f')
        #self.logger.info(date+':'+msg.type_id)
        #if self.decode_type == self.XML:
        #    # print msg.data.decode_xml()
        #    self.logger.info(msg.data.decode_xml())
        #elif self.decode_type == self.JSON:
        #    # print msg.data.decode_json()
        #    self.logger.info(msg.data.decode_json())
        #elif self.decode_type == self.NORMAL:
        #    self.logger.info(msg.data.decode())

    def show(self):
        print "***show ** %d" %(len(self.__msg_log))
	app = MyApp(self.__msg_log)
	app.set_logs(self.__msg_log)
    	app.MainLoop()

class MyPanel(wx.Panel):
    def __init__(self, parent, logs):
        super(MyPanel, self).__init__(parent, size=(1000,800))

        self.data = logs

        print ('Number of logs : %d ' %len(logs))
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.grid = wx.grid.Grid(self, pos=(0,0), size=(400,800))
        self.grid.CreateGrid(max(25, len(self.data)), 2)
        self.grid.SetColLabelValue(0, "Timestamp")
        self.grid.SetColLabelValue(1, "Type ID")
        #self.grid.SetColLabelValue(5, "Data")


        #init grid
        for i in range(len(self.data)):
            self.grid.SetCellValue(i, 0, str(self.data[i].timestamp))
            self.grid.SetCellValue(i, 1, str(self.data[i].type_id))

        self.grid.Bind(wx.grid.EVT_GRID_CELL_LEFT_CLICK, self.onRowClick)

        self.text = wx.TextCtrl(self, pos=(450,1), size=(600,800), style=wx.TE_MULTILINE)
        self.text.SetBackgroundColour(wx.LIGHT_GREY)
	sizer.Add(self.grid, 2)
	sizer.Add(self.text, 3)

	self.SetSizer(sizer)


    def onRowClick(self, event):
        row = event.GetRow()
        print 'Clicked %d ' %row
	if (isinstance(self.data[row].data, DMLogPacket)):
	    self.text.SetValue(str(self.data[row].data.decode_xml()))
	else:
	    self.text.SetValue(str(self.data[row].data))
	event.Skip()

class MyFrame(wx.Frame):
    def __init__(self, parent, logs, title=" "):
        super(MyFrame, self).__init__(parent, title=title, size=(1000,600))
	IDB_OPEN = wx.NewId()
	self.toolbar = self.CreateToolBar()
	self.openBtn = self.toolbar.AddLabelTool(wx.ID_ANY, "Open", wx.Bitmap(sys.exec_prefix+"/mobile_insight/icons/open.png"))
	
	self.toolbar.Realize()
	self.Bind(wx.EVT_TOOL, self.OpenHandler, self.openBtn)
	self.CreateStatusBar()	
        self.panel = MyPanel(self, logs)
	menuBar = wx.MenuBar()
        filem = wx.Menu()
        menuBar.Append(filem, "&File")
	self.SetMenuBar(menuBar)

    def OpenHandler(self, evt):
	openFileDialog = wx.FileDialog(self, "Select log file", "", "", "Log files (*.mi2log)|*.mi2log", wx.FD_OPEN | wx.FILE_MUST_EXIST) 
	if openFileDialog.ShowModal() == wx.ID_CANCEL:
	    return
class MyApp(wx.App):
    def __init__(self, logs):
	print "***__ init___** "
	self._logs = logs
	super(MyApp, self).__init__()
	print "Number of logs %d" %len(self._logs)

    def set_logs(self, logs):
	print "****_set _logs ***"
        self._logs = logs

    def OnInit(self):
	print "*** __ onInit___ ***"
	print "Number of logs %d" %len(self._logs)
        self.frame = MyFrame(None, self._logs, title="Mobile Insight Events")
        self.frame.Show();

        self.Bind(wx.EVT_BUTTON, self.OnButton)
        return True

    def OnButton(self, event):
        button = event.EventObject
        print("Button (%s) event at App!" % button.Label)
        event.Skip()





