#!/usr/bin/python
# Filename: profile.py
"""
Profile abstraction

Author: Yuanjie Li
"""

from jnius import autoclass

class Node(object):
    '''
    A helper class for the ProfileHierarchy
    '''
    def __init__(self,name):
        self.name = name
        self.children = [] # A node list to its children


    def add(self,child):
        '''
        Add a child 

        :param child: a child node
        :type child: string

        :returns: the added node
        '''
        child_node = Node(child)
        self.children.append(child_node)
        return child_node

    def is_leaf(self):
        '''
        Return true if this node is a leaf

        :returns: True if is a leaf, False otherwise
        '''
        return not self.children


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
        self.__root = Node(root)


    def get_root(self):
        '''
        Return the root node
        '''
        return self.__root

    def get_node(self,name):
        '''
        Get the node based on the hierarchical name

        :param name: a hierarchical name separated by '.' (e.g., LteRrc.Reconfig.Sib)
        :type name: string

        :returns: the Node that corresponds to this name, or None if it does not exist
        '''
        nodes = name.split('.')
        count = 0
        cur_node = self.__root

        if nodes[count] != cur_node.name:
            return None

        while True:

            # current node is matched, search the next hierarchy
            count = count + 1

            # the hierarchy is finished, then return the current node
            if count == len(nodes):
                return cur_node

            match = False
            # otherwise, update the node to the one that matches the new name
            for child in cur_node.children:
                if child.name == nodes[count]:
                    cur_node = child
                    match = True
                    break

            # if no children matches, return None
            if not match:
                return None



class Profile(object):
    
    '''
    Profile abstraction

    Given the profile hierarchy, this abstraction achieves

        - Automatically create Tables for profile

        - Enforce query with hierarchical name (e.g., LTERrcAnalyzer.Reconfig.Drx.long_drx)

        - Update profile values
    '''

    def __init__(self, profile_hierarchy):
        '''
        Initialization of the Profile

        :param profile_hierarchy: the profiel hierarchy tree
        '''
        self.__profile_hierarchy = profile_hierarchy
        self.__db = None
        self.__build_db()

       
    def __create_table(self,node):
    	'''
    	Create SQL tables for the node

    	:param node: a node in the profile hierarchy
    	:type node: Node
    	'''

        if node.is_leaf():
            return

        sql_cmd = 'CREATE TABLE IF NOT EXISTS ' + node.name + "(id,"
        for child in node.children:
            sql_cmd = sql_cmd + child.name + ","
        sql_cmd = sql_cmd + ")"
        self.__db.execSQL

        # Recursion
        for child in node.children:
            self.__create_table(child)    

    def __build_db(self):
        '''
        Build the internal DBs for the profile
        '''

        if self.__profile_hierarchy is None:
            self.__db = None
        else:
            #setup internal database
            root = self.__profile_hierarchy.get_root()
            activity = autoclass('org.renpy.android.PythonActivity')
            self.__db = activity.mActivity.openOrCreateDatabase(root.name+'.db',0,None)

            self.__create_table(root)