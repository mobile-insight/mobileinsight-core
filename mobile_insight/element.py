#!/usr/bin/python
# Filename: element.py
"""
Basic abstractions for the trace collector and analyzer.

Author: Yuanjie Li
"""

__all__ = ["Event", "Element"]


class Event(object):
    '''The event is used to trigger the analyzer and perform some actions.

    The event can be raised by a trace collector (a message) or an analyzer.
    An event is a triple of (timestamp, type, data).
    '''
    def __init__(self,timestamp,type_id,data):
        self.timestamp = timestamp
        self.type_id = type_id
        self.data = data


class ProfileHierarchy(object):

    '''An abstraction for analyzers to declare the profile it maintains. 

    Given this hierarchy, the analyzer automatically builds underlying database, 
    and enforces query with hierarchical name (e.g., LTERrcAnalyzer.Reconfig.Drx.long_drx)
    '''

    def __init__(self,root):
        '''
        Initialization of the profile hierarchy

        :param root: the root profile table name
        :type root: string
        '''
        self.__root = root
        self.__tree = {} #node_name -> list of children names
        self.__tree[root] = []

    def add_node(self, parent, child):
        '''
        Add a new node to an existing node in the tree

        :param parent: the parent node 
        :type root: string
        :param child: the new node to add_node
        :type child: string

        :returns: True if succeeds, Falase otherwise
        '''

        if not parent in self.__tree:
            #parent does not exist
            return False

        if child in self.__tree[parent]:
            #child already in the tree
            return True
        else:
            self.__tree[parent].append(child)
            self.__tree[child]=[]
            return True

    def add_nodes(self, parent, child_list):
        '''
        Add a list of nodes to a common parent

        :param parent: the parent node 
        :type root: string
        :param child_list: the new node to add_node
        :type child_list: string list

        :returns: True if succeeds, Falase otherwise
        '''
        if not parent in self.__tree:
            return False

        for child in child_list:
            self.add_node(parent,child)
        return True

    #node type
    NOT_EXIST = 0
    ROOT = 1
    LEAF = 2
    NON_LEAF = 3

    def check_node(self, node):
        '''
        Check if a node exists, and its type 

        :param node: the name of the node
        :type node: string

        :returns: the node type (NOT_EXIST, ROOT, NON_LEAF or LEAF)
        '''
        if node ==  self.__root:
            return self.ROOT
        elif node in self.__tree:
            if self.__tree[node]:
                return self.NON_LEAF
            else:
                return self.LEAF
        else:
            return self.NOT_EXIST

    def get_root(self):
        '''
        Return the root name

        :returns: the root name
        '''
        return self.__root

    def get_children(self,node):
        '''
        Return the children list

        :param node: the node name

        :returns: the children list, or None if the node does not exist
        '''
        if node in self.__tree:
            return self.__tree[node]
        else:
            return None


class Element(object):
    '''The parent class to derive trace collectors and analyzers.
    '''

    def __init__(self):
        self.from_list={}   #module that it depends, module->callback
        self.to_list=[] #list of other module that call for this module

    def send(self,event):
        """
        Raise an event to all Analyzers in from_analyzer_list

        :param event: the event to be sent
        """
        for module in self.to_list:
            module.recv(self,event)

    def recv(self,module,event):
        """
        Upon receiving an event from module, trigger associated callbacks

        This method should be overwritten by the analyzer and trace collector

        :param module: the module who raises the event
        :param event: the event to be received
        """
        pass
