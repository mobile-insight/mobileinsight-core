#!/usr/bin/python

"""
Python GUI for MobileInsight
Author: Moustafa Alzantot
Date : Feb 26, 2016
"""

import sys
import wx
import wx.grid
import wx.animate
from threading import Thread
from random import random 
from datetime import datetime, timedelta

import pandas as pd
import matplotlib
from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg as FigureCanvas
# from matplotlib.backends.backend_wx import NavigationToolbar2Wx, wxc
from matplotlib.backends.backend_wx import NavigationToolbar2Wx
from matplotlib.figure import Figure

import xml.dom.minidom

from mobile_insight.analyzer import LogAnalyzer
from mobile_insight.monitor.dm_collector.dm_endec.dm_log_packet import DMLogPacket


ID_FILE_OPEN = wx.NewId()
ID_FILE_EXIT = wx.NewId()
ID_TB_OPEN = wx.NewId()
ID_TB_FILTER = wx.NewId()
ID_TB_SEARCH = wx.NewId()
ID_TB_TIME = wx.NewId()
ID_TB_RESET = wx.NewId()
ID_TB_GRAPH = wx.NewId()

EVT_RESULT_ID = wx.NewId()


def EVT_RESULT(win, func):
    win.Connect(-1, -1, EVT_RESULT_ID, func)


class ResultEvent(wx.PyEvent):
    def __init__(self, data):
        wx.PyEvent.__init__(self)
        self.SetEventType(EVT_RESULT_ID)
        self.data = data

class CanvasDialog(wx.Dialog):
    def __init__(self, parent, myDf):
        wx.Dialog.__init__(self, parent, -1)
        self.SetBackgroundColour(wx.NamedColour("WHITE"))

        self.df = myDf
        self.figure = Figure()
        self.canvas = FigureCanvas(self, -1, self.figure)
        self.axes = self.figure.add_subplot(111)
        self.axes.clear()

        types = list(myDf.TypeId.unique())
        colors =['r', 'g', 'b', 'k', 'c', 'y']
        k = 0
        for type_id in types:
            myDf.loc[myDf.TypeId==type_id].groupby(pd.TimeGrouper('s')).count()['TypeId'].plot(ax=self.axes, label=type_id, color=colors[k])
            k += 1
        #myDf.to_csv('~/output.csv')
        self.axes.legend(loc='best')
        self.axes.set_xlabel("Time")
        self.axes.set_ylabel("Number of messages")
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.canvas, 1, wx.LEFT | wx.TOP | wx.GROW)
        self.SetSizer(self.sizer)
        self.Fit()


class ProgressDialog(wx.Dialog):
    def __init__(self, parent):
        wx.Dialog.__init__(self, parent, -1, style = wx.NO_BORDER)
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        
        gif = wx.animate.GIFAnimationCtrl(self, -1, "icons/progress.gif", pos=(0, 0), size=(-1, -1))
        gif.GetPlayer().UseBackgroundColour(True)
        gif.Play()

        mainSizer.Add(gif,  wx.EXPAND | wx.ALL)
        self.SetSizer(mainSizer)
        self.Fit()

class TimeWindowDialog(wx.Dialog):
    def __init__(self, parent, start_time, end_time):
        wx.Dialog.__init__(self, parent, -1)
        sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetTitle("Time Window")
        self.start_label = wx.StaticText(self, -1, label="...", style=wx.BOLD)
        self.end_label = wx.StaticText(self, -1, label="...", style=wx.BOLD)
        self.window_label = wx.StaticText(self, -1, "\t to \t")
        self.start_label.SetFont(wx.Font(11,  wx.DEFAULT, wx.BOLD, wx.NORMAL))
        self.window_label.SetFont(wx.Font(11, wx.DEFAULT, wx.ITALIC, wx.NORMAL))
        self.end_label.SetFont(wx.Font(11,  wx.DEFAULT, wx.BOLD, wx.NORMAL))

        labelSizer = wx.BoxSizer(wx.HORIZONTAL)
        labelSizer.Add(self.start_label, 0, wx.ALL | wx.EXPAND, 3)
        labelSizer.Add(self.window_label, wx.ALL, 1)
        labelSizer.Add(self.end_label, 0, wx.ALL | wx.EXPAND, 3)

        self.btns = self.CreateSeparatedButtonSizer(wx.OK | wx.CANCEL)
        start_sizer = wx.BoxSizer(wx.HORIZONTAL)
        start_sizer.Add(wx.StaticText(self, -1, "Start : "), 0, wx.ALL, 1)
        self.start_slider = wx.Slider(self, -1, 0, 0, 100,wx.DefaultPosition, (250,-1),wx.SL_HORIZONTAL )
        start_sizer.Add(self.start_slider, 0, wx.ALL | wx.EXPAND, 5)
        self.Bind(wx.EVT_SLIDER, self.start_slider_update, self.start_slider)

        end_sizer = wx.BoxSizer(wx.HORIZONTAL)
        end_sizer.Add(wx.StaticText(self, -1, "End: "), 0, wx.ALL, 1)
        self.end_slider = wx.Slider(self, -1, 100, 0, 100,wx.DefaultPosition, (250,-1),wx.SL_HORIZONTAL )
        end_sizer.Add(self.end_slider, 0, wx.ALL | wx.EXPAND, 5)
        self.Bind(wx.EVT_SLIDER, self.end_slider_udpate, self.end_slider)


        self.start_time = start_time
        self.cur_end = end_time
        self.cur_start = self.start_time
        self.unit_seconds = (end_time - start_time).total_seconds()/100.0;

        self.updateUI()
        sizer.Add(labelSizer, 0, wx.ALL | wx.EXPAND, 5)
        sizer.Add(start_sizer, 0, wx.ALL | wx.EXPAND, 5)
        sizer.Add(end_sizer, 0, wx.ALL | wx.EXPAND, 5)
        sizer.Add(self.btns, 0, wx.ALL | wx.EXPAND, 5)
        self.SetSizer(sizer)
        self.Fit()

    def start_slider_update(self, event):
        delta_seconds = self.start_slider.GetValue() * self.unit_seconds
        self.cur_start  = self.start_time + timedelta(seconds=int(delta_seconds))
        self.updateUI()


    def end_slider_udpate(self, event):
        delta_seconds = self.end_slider.GetValue() * self.unit_seconds
        self.cur_end  = self.start_time + timedelta(seconds=int(delta_seconds))
        self.updateUI()

    def updateUI(self):
        self.start_label.SetLabel(format("%s" %(self.cur_start)))
        self.end_label.SetLabel(format("%s" %(self.cur_end)))





class MyMCD(wx.Dialog):
    def __init__(self, parent, message, caption, choices=[]):
        wx.Dialog.__init__(self, parent, -1)
        self.SetTitle(caption)
        sizer = wx.BoxSizer(wx.VERTICAL)
        self.message = wx.StaticText(self, -1, message)
        self.clb = wx.CheckListBox(self, -1, choices = choices)
        self.chbox = wx.CheckBox(self, -1, 'Select all')
        self.btns = self.CreateSeparatedButtonSizer(wx.OK | wx.CANCEL)
        self.Bind(wx.EVT_CHECKBOX, self.EvtChBox, self.chbox)

        sizer.Add(self.message, 0, wx.ALL | wx.EXPAND, 5)
        sizer.Add(self.clb, 1, wx.ALL | wx.EXPAND, 5)
        sizer.Add(self.chbox, 0, wx.ALL | wx.EXPAND, 5)
        sizer.Add(self.btns, 0, wx.ALL | wx.EXPAND, 5)
        self.SetSizer(sizer)
#        self.Fit()

    def GetSelections(self):
        return self.clb.GetChecked()

    def EvtChBox(self, event):
        state = self.chbox.IsChecked()
        for i in range(self.clb.GetCount()):
            self.clb.Check(i, state)

class WindowClass(wx.Frame):

    def __init__(self, *args, **kwargs):
        super(WindowClass, self).__init__(*args, **kwargs)

        self.min_time = datetime.strptime("3000 Jan 1" ,'%Y %b %d')
        self.max_time = datetime.strptime("1900 Jan 1", '%Y %b %d')
        self.basicGUI()

    def basicGUI(self):

        self._log_analyzer = LogAnalyzer(self.OnReadComplete)
        menuBar = wx.MenuBar()
        fileButton = wx.Menu()
        editButton = wx.Menu()

        openItem = fileButton.Append(ID_FILE_OPEN, "Open", "Open log file")
        exitItem = fileButton.Append(ID_FILE_EXIT, "Exit", "Exit application")

        menuBar.Append(fileButton, 'File')
        menuBar.Append(editButton, "Edit")
        self.SetMenuBar(menuBar)

        self.Bind(wx.EVT_MENU, self.Quit, exitItem)
        self.Bind(wx.EVT_MENU, self.Open, openItem)

        ### Toolbar
        self.toolbar = self.CreateToolBar(wx.TB_FLAT | wx.TB_TEXT | wx.TB_HORIZONTAL |
            wx.NO_BORDER)

        toolbar_open = self.toolbar.AddLabelTool(ID_TB_OPEN, "Open", wx.Bitmap("icons/open.png"))
        self.toolbar.AddSeparator()
        toolbar_filter = self.toolbar.AddLabelTool(ID_TB_FILTER, "Filter", wx.Bitmap("icons/filter.png"))
        self.toolbar.AddSeparator()
        toolbar_search = self.toolbar.AddLabelTool(ID_TB_SEARCH, "Search", wx.Bitmap("icons/search.png"))
        self.toolbar.AddSeparator()
        toolbar_time = self.toolbar.AddLabelTool(ID_TB_TIME, "Time Window", wx.Bitmap("icons/time.png"))
        self.toolbar.AddSeparator()
        toolbar_reset = self.toolbar.AddLabelTool(ID_TB_RESET, "Reset", wx.Bitmap("icons/reset.png"))
        # self.toolbar.AddSeparator()
        # toolbar_graph = self.toolbar.AddLabelTool(ID_TB_GRAPH, "Graph", wx.Bitmap("icons/graph.png"))
        self.toolbar.AddSeparator()
        toolbar_about = self.toolbar.AddLabelTool(ID_TB_GRAPH, "About", wx.Bitmap("icons/about.png"))

        self.Bind(wx.EVT_TOOL, self.Open, toolbar_open)
        self.Bind(wx.EVT_TOOL, self.OnFilter, toolbar_filter)
        self.Bind(wx.EVT_TOOL, self.OnSearch, toolbar_search);
        self.Bind(wx.EVT_TOOL, self.OnTime, toolbar_time)
        self.Bind(wx.EVT_TOOL, self.OnReset, toolbar_reset)
        # self.Bind(wx.EVT_TOOL, self.OnGraph, toolbar_graph)
        self.Bind(wx.EVT_TOOL, self.OnAbout, toolbar_about)
        self.toolbar.Realize()

        #### Main Panel
        panel = wx.Panel(self, -1, size=(-1, -1),  style=wx.BORDER_RAISED)
        mainSizer = wx.BoxSizer(wx.HORIZONTAL)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        self.grid = wx.grid.Grid(self) #
        self.grid.CreateGrid(50, 2)
        self.grid.SetSelectionMode(1) # 1 is Select Row


        self.grid.Bind(wx.grid.EVT_GRID_SELECT_CELL, self.OnGridSelect)
        self.grid.SetColLabelValue(0, "Timestamp")
        self.grid.SetColLabelValue(1, "Type ID")
        

        hbox.Add(self.grid, 5, wx.EXPAND | wx.ALL, 10)

        leftPanel = wx.Panel(self, -1, size=(-1,-1), style=wx.BORDER_RAISED)

        leftbox = wx.BoxSizer(wx.VERTICAL)
        self.status_text = wx.StaticText(leftPanel, label="Welcome to MobileInsight!\n\nMobileInsight is a Python (2.7) package for mobile network monitoring and analysis on the end device. It is developed by UCLA Wireless Networking Group (Wing) and OSU MSSN lab.", style=wx.ALIGN_LEFT)
        self.details_text = wx.TextCtrl(leftPanel, style=wx.ALIGN_LEFT|wx.TE_MULTILINE)

        leftbox.Add(self.status_text, 1, wx.EXPAND | wx.HORIZONTAL)
        leftbox.Add(self.details_text, 3, wx.EXPAND)

        leftPanel.SetSizer(leftbox)
        hbox.Add(leftPanel, 4, wx.EXPAND | wx.ALL, 10)
        self.grid.SetColSize(0, 200)
        self.grid.SetColSize(1, 300)
        self.grid.ForceRefresh()

        panel.SetSizer(hbox)

        
        mainSizer.Add(panel, 1, wx.EXPAND,0)
        self.SetSizer(mainSizer)
        self.statusbar = self.CreateStatusBar()
        self.Bind(wx.EVT_CLOSE, self.Quit)
        self.SetTitle("MobileInsight")
        self.SetSize((1200, 800))
        self.Centre()
        self.Show(True)

        self.data = None

        EVT_RESULT(self, self.OnResult)


    def OnResult(self, event):
        if self.progressDialog:
            self.progressDialog.EndModal(wx.ID_CANCEL)
            self.progressDialog.Destroy()

        data = event.data
        if data:
            self.statusbar.SetStatusText("Read %d logs" %len(data))
            self.data = data
            self.data_view = self.data
            self.SetupGrid()

    def Open(self, e):
        openFileDialog = wx.FileDialog(self, "Open Log file","", "",
            "log files (*.mi2log) |*.mi2log| All files |*.*", wx.FD_OPEN | wx.FD_FILE_MUST_EXIST)
        if (openFileDialog.ShowModal() == wx.ID_OK):
            print 'Selected %s' %openFileDialog.GetPath()
            try:
                self.grid.ClearGrid()


                #thread.start_new_thread(openFile,(openFileDialog.GetPath(),))
                t = Thread(target = self.openFile, args=(openFileDialog.GetPath(),))
                self.progressDialog = ProgressDialog(self)
                t.start()
                self.progressDialog.ShowModal()

                self.SetTitle(openFileDialog.GetPath())

            except e:
                print "Error while opening file.", e
            #if (random() > 0.5):
            #    self.SetupGrid(self.data1)
            #else:
            #    self.SetupGrid(self.data2)

    def OnFilter(self, e):
        types = list(self._log_analyzer.supported_types)
        checkboxDialog = MyMCD(self, "Filter", "", types)
        if (checkboxDialog.ShowModal()==wx.ID_OK):
            selectedTypes = [types[x] for x in checkboxDialog.GetSelections()]
            self.data_view = [x for x in self.data if x["TypeID"] in selectedTypes]
            self.SetupGrid()

    def OnTime(self, e):
        timewindowDialog = TimeWindowDialog(self, self.min_time, self.max_time)
        if (timewindowDialog.ShowModal() == wx.ID_OK):
            select_start = timewindowDialog.cur_start
            select_end = timewindowDialog.cur_end
            self.data_view = [x for x in self.data_view if datetime.strptime(x["Timestamp"], '%Y-%m-%d  %H:%M:%S.%f') >= select_start and datetime.strptime(x["Timestamp"], '%Y-%m-%d  %H:%M:%S.%f') <= select_end]
            self.SetupGrid()

    def OnReset(self, e):
        if self.data:
            self.data_view = self.data
            self.SetupGrid()

    def openFile(self, filePath):
        self._log_analyzer.AnalyzeFile(filePath)

    def OnGraph(self, e):
        time_list = []
        type_list = []
        if not hasattr(self,'data_view'):
            return    
        n = len(self.data_view)
        for k in range(n):
            time_list += [datetime.strptime(self.data_view[k]["Timestamp"], '%Y-%m-%d  %H:%M:%S.%f')]
            type_list += [(self.data_view[k]["TypeID"])]
        myDf = pd.DataFrame({'Timestamp' : time_list,
            'TypeId': type_list})
        myDf.index = myDf['Timestamp']
        #df.plot('Timestamp', 'TypeId')

        dlg = CanvasDialog(self, myDf)
        #myDf.groupby(pd.TimeGrouper('s')).count().plot()
        dlg.ShowModal()


    def OnSearch(self, e):
        search_dlg = wx.TextEntryDialog(self, "Search for", "", "", style=wx.OK | wx.CANCEL)
        if (search_dlg.ShowModal() == wx.ID_OK):
            keyword = search_dlg.GetValue()
            self.data_view = [x for x in self.data_view if keyword in x["Payload"]]
            self.SetupGrid()
        search_dlg.Destroy()

    def OnAbout(self, e):
        about_text = ('MobileInsight 2.0 GUI\n' 
                   + 'UCLA Wing Group & OSU MSSN Lab\n\n' 
                   + 'Developers: \n   Moustafa Alzantot, \n    Priyanka Avinash Kachare,\n'
                   + '    Michael Ivan, \n    Yuanjie Li\n\n'
                   + 'Copyright 2014-2016')
        search_dlg = wx.MessageDialog(self, about_text, "About MobileInsight GUI", wx.OK)
        search_dlg.ShowModal()

    def OnGridSelect(self, e):
        #self.statusbar.SetStatusText("Selected %d" %e.GetRow())
        row = e.GetRow()
        if (row < len(self.data_view)):
            self.status_text.SetLabel("Time Stamp : %s    Type : %s" %(str(self.data_view[row]["Timestamp"]), str(self.data_view[row]["TypeID"])))
            # self.details_text.SetValue(str(self.data_view[row]["Payload"]))
            val = xml.dom.minidom.parseString(str(self.data_view[row]["Payload"]))
            pretty_xml_as_string = val.toprettyxml(indent="  ",newl="\n")
            self.details_text.SetValue(pretty_xml_as_string)
        e.Skip()

    def Quit(self, e):
        self.Destroy()

    def OnReadComplete(self):
        evt = ResultEvent(self._log_analyzer.msg_logs)
        wx.PostEvent(wx.GetApp().frame, evt)
        


    def SetupGrid(self):
        self.min_time = datetime.strptime("3000 Jan 1" ,'%Y %b %d')
        self.max_time = datetime.strptime("1900 Jan 1", '%Y %b %d')
        
        n = len(self.data_view)
        # self.grid.CreateGrid(max(25, n), 2)
        if n>self.grid.GetNumberRows():
            self.grid.InsertRows(0,n-self.grid.GetNumberRows())
        else:
            self.grid.DeleteRows(0,self.grid.GetNumberRows()-n)

        self.grid.ClearGrid()
        self.grid.SetColLabelValue(0, "Timestamp")
        self.grid.SetColLabelValue(1, "Type ID")
        for i in range(n):
            try:
                cur_time = datetime.strptime(self.data_view[i]["Timestamp"], '%Y-%m-%d  %H:%M:%S.%f')
            except Exception, e:
                cur_time = datetime.strptime(self.data_view[i]["Timestamp"], '%Y-%m-%d  %H:%M:%S')
            self.min_time = min(self.min_time, cur_time)
            self.max_time = max(self.max_time, cur_time)
            self.grid.SetCellValue(i, 0, str(self.data_view[i]["Timestamp"]))
            self.grid.SetCellValue(i, 1, str(self.data_view[i]["TypeID"]))
            self.grid.SetReadOnly(i, 0)
            self.grid.SetReadOnly(i, 1)
        #self.grid.Bind(wx.grid.EVT_GRID_CELL_LEFT_CLICK, self.onRowClick)






def main():
    wx.Log.SetLogLevel(0)
    app = wx.App()
    app.frame = WindowClass(None)
    app.MainLoop()

main()