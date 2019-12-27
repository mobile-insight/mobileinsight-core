#!/usr/bin/python
# Filename: profile.py
"""
Profile abstraction

Author: Yuanjie Li
"""

is_android = False
try:
    from jnius import autoclass  # For Android
    is_android = True
except Exception as e:
    import sqlite3  # Laptop version
    is_android = False

import ast
import os


__all__ = ["ProfileHierarchy", "Profile"]


'''
To support per-level ID, we update the following hierarchy

LteRrc:87/Sib/Inter_freq:5780/ThreshXHigh

This is similar to HTTP, except that every level can define another ID for query (list)

The hierarchy ID is mandatory for root, but optional for non-root nodes
'''


class ProfileNode(object):
    '''
    A profile node for the ProfileHierarchy tree
    '''

    def __init__(self, name, id_required):
        '''
        Initialization of a Profile node

        :param name: the name of this hierarchy
        :type name: string
        :param id_required: specify if this node has a key (id), e.g., Inter_freq:5780
        :type id_required: boolean
        '''
        self.name = name
        self.id_required = id_required
        self.children = {}  # A node list to its children

    def add(self, child, id_required):
        '''
        Add a child node

        :param child: a child node
        :type child: string
        :param id_required: specify if this node has a key (id), e.g., Inter_freq:5780
        :type id_required: boolean

        :returns: the added node
        '''
        child_node = ProfileNode(child, id_required)
        self.children[child] = child_node
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

    Example construction: consider the following RRC profile hierarchy

        LteRrc
          - Sib
            - Inter_freq (id_required, e.g., Inter_freq:5780)
              - ThreshXHigh
              - ThreshXLow
          - Reconfig
            - Drx
              - Short_drx
              - Long_drx

    The following code constructs such a profile hierachy:

        LteRrcProfile = ProfileHierarchy('LteRrc')
        root = LteRrcProfile.get_root();
        sib=root.add('Sib',False);
          inter_freq=sib.add('Inter_freq',True) #ID required
        reconfig=root.add('Reconfig',False);
          measconfig=reconfig.add('MeasConfig',False)
          drx=reconfig.add('Drx',False);
            drx.add('Drx_short',False);
            drx.add('Drx_long',False);
    '''

    def __init__(self, root):
        '''
        Initialization of the profile hierarchy

        :param root: the root profile table name
        :type root: string
        '''
        self.__root = ProfileNode(root, True)  # Root MUST have a unique ID

    def get_root(self):
        '''
        Return the root node
        '''
        return self.__root

    def get_node(self, name):
        '''
        Get the node based on the hierarchical name

        :param name: a hierarchical name separated by '.' (e.g., LteRrc:87.Sib)
        :type name: string

        :returns: the Node that corresponds to this name, or None if it does not exist
        '''
        nodes = name.split('.')
        count = 0
        cur_node = self.__root

        node_split = nodes[count].split(':')
        if node_split[0] != cur_node.name or len(node_split) == 1:
            return None

        while True:

            # current node is matched, search the next hierarchy
            count = count + 1

            # the hierarchy is finished, then return the current node
            if count == len(nodes):
                return cur_node

            match = False
            # otherwise, update the node to the one that matches the new name
            for child in list(cur_node.children.values()):

                node_split = nodes[count].split(':')

                if child.name == node_split[0]:
                    if child.id_required and len(node_split) == 1:
                        # The mandatory ID is unavailable
                        return None
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

    def __create_table(self, node):
        '''
        Create SQL tables for the node

        :param node: a node in the profile hierarchy
        :type node: Node
        '''
        sql_cmd = 'CREATE TABLE IF NOT EXISTS ' + \
            node.name + "(id,profile,primary key(id))"
        if is_android:
            self.__db.execSQL(sql_cmd)
        else:
            self.__db.execute(sql_cmd)
            self.__conn.commit()

    def __get_root_name(self):
        return self.__profile_hierarchy.get_root().name

    def __build_db(self):
        '''
        Build the internal DBs for the profile
        '''
        if self.__profile_hierarchy is None:
            self.__db = None
        else:
            # setup internal database
            root = self.__profile_hierarchy.get_root()
            if is_android:
                Environment = autoclass("android.os.Environment")
                state = Environment.getExternalStorageState()
                if not Environment.MEDIA_MOUNTED == state:
                    self.__db = None
                    return

                sdcard_path = Environment.getExternalStorageDirectory().toString()
                DB_PATH = os.path.join(sdcard_path, "mobileinsight/dbs")
                activity = autoclass('org.kivy.android.PythonActivity')
                if activity.mActivity:
                    self.__db = activity.mActivity.openOrCreateDatabase(
                        os.path.join(DB_PATH, root.name + '.db'), 0, None)
                else:
                    service = autoclass('org.kivy.android.PythonService')
                    self.__db = service.mService.openOrCreateDatabase(
                        os.path.join(DB_PATH, root.name + '.db'), 0, None)
            else:
                self.__conn = sqlite3.connect(root.name + '.db')
                self.__db = self.__conn.cursor()

            self.__create_table(root)

    def query(self, profile_name):
        '''
        Query the profile value with a hierarchical name.
        Example: self.query('cell_id=87','LteRrc:87.Reconfig.Drx.Short_drx')

        :param profile_name: a hierarcical name separated by '.'. If id is required, it's separated by ":"
        e.g., "LteRrc:87.Sib.Inter_freq:5780.ThreshXHigh"
        :type profile_name: string
        :returns: value list that satisfies the query, or None if no such field (id not exist, incomplete record, etc.)
        '''

        try:
            # Step 1: check if the name conforms to the hierarchy
            if self.__profile_hierarchy is None:  # no profile defined
                return None
            # Check if the field to query is valid
            profile_node = self.__profile_hierarchy.get_node(profile_name)
            if profile_node is None:
                return None

            profile_nodes = profile_name.split('.')

            # Step 2: extract the raw profile
            # NOTE: root profile MUST have a id
            sql_cmd = "select profile from " + self.__get_root_name() + " where id=\"" + \
                profile_nodes[0].split(":")[1] + "\""

            if is_android:
                sql_res = self.__db.rawQuery(sql_cmd, None)
            else:
                sql_res = self.__db.execute(sql_cmd).fetchall()

            # if sql_res.getCount()==0: #the id does not exist
            if (is_android and sql_res.getCount() == 0) or (
                    not is_android and len(sql_res) == 0):
                return None

            if is_android:
                sql_res.moveToFirst()
                # convert string to dictionary
                res = ast.literal_eval(sql_res.getString(0))
            else:
                res = ast.literal_eval(sql_res[0][0])

            # Step 3: extract the result from raw profile
            for i in range(1, len(profile_nodes)):
                if res is None:  # no profile
                    break
                profile_node_split = profile_nodes[i].split(":")
                res = res[profile_node_split[0]]
                if len(profile_node_split) > 1:
                    res = res[profile_node_split[1]]
            return res
        except BaseException:            # TODO: raise warnings
            return False

    def update(self, profile_name, value_dict):
        '''
        Update a profile value

        Example 1: self.update('LteRrc:87.Reconfig.Drx',{Drx_short:1,Drx_long:5})
        Example 2: self.update('LteRrc:87.Sib.Inter_freq:5780',{ThreshXHigh:1,ThreshXLow:2})

        If the id does not exist, create a new item in the root, with specified values and all other fields as "null"

        Otherwise, update the specified field values, and keep the ramaining values unchanged.

        The update operation is atomic. No partial update would be performed

        :param profile_name: a hierarcical name separated by '.' (e.g., LteRrc.Reconfig.Drx)
        :type profile_name: string
        :param value: a field_name->value dictionary of the specified updated values.
        All the field names should appear in the profile_name.
        :type value: string->string dictionary
        :returns: True if the update succeeds, False otherwise
        '''

        try:

            # Step 1: check if the name conforms to the hierarchy
            if not self.__profile_hierarchy:  # no profile defined
                raise Exception('No profile defined')
                return False
            # Check if the field to update is valid
            test_node = self.__profile_hierarchy.get_node(profile_name)
            if not test_node:
                raise Exception('Invalid update: ' + profile_name)
                return False
            # Check the value fileds to update are indeed included based on
            # hierarchy
            for field_name in value_dict:
                if field_name not in test_node.children:
                    # Invalid node
                    raise Exception('Invalid update field: ' + str(value_dict))
                    return False

            profile_nodes = profile_name.split('.')

            # Step 2: check if the id exists or not
            sql_cmd = "select profile from " + self.__get_root_name() + " where id=\"" + \
                profile_nodes[0].split(":")[1] + "\""
            if is_android:
                sql_res = self.__db.rawQuery(sql_cmd, None)
            else:
                sql_res = self.__db.execute(sql_cmd).fetchall()

            # if not query_res:
            # if sql_res.getCount()==0:
            if (is_android and sql_res and sql_res.getCount() == 0) or (
                    not is_android and len(sql_res) == 0):
                # The id does not exist. Create a new record

                query_res = {}
                res = query_res
                profile_node = self.__profile_hierarchy.get_root()
                # Init: all root's children are not initialized
                for child in profile_node.children:
                    res[child] = None

                # Go along hierarchy, init the remaining children
                for i in range(1, len(profile_nodes)):
                    profile_node_split = profile_nodes[i].split(":")
                    profile_node = profile_node.children[profile_node_split[0]]
                    res[profile_node_split[0]] = {}
                    res = res[profile_node_split[0]]
                    if profile_node.id_required:
                        res[profile_node_split[1]] = {}
                        res = res[profile_node_split[1]]
                    for child in profile_node.children:
                        res[child] = None

                for item in value_dict:
                    res[item] = value_dict[item]

                # Insert the new record into table
                sql_cmd = "insert into " + self.__get_root_name() + "(id,profile) values(\"" + \
                    profile_nodes[0].split(":")[1] + "\"," + "\"" + str(query_res) + "\")"
                if is_android:
                    # print "Yuanjie: execSQL"
                    self.__db.execSQL(sql_cmd)
                else:
                    self.__db.execute(sql_cmd)
                    self.__conn.commit()

                return True
            else:
                if is_android:
                    sql_res.moveToFirst()
                    query_res = ast.literal_eval(
                        sql_res.getString(0))  # convert string to dictionary
                else:
                    query_res = ast.literal_eval(sql_res[0][0])
                # The id exists. Update the record
                res = query_res
                profile_node = self.__profile_hierarchy.get_root()

                for i in range(1, len(profile_nodes)):
                    profile_node_split = profile_nodes[i].split(":")
                    if res[profile_node_split[0]] is not None:
                        res = res[profile_node_split[0]]
                        if len(profile_node_split) > 1:
                            if profile_node_split[1] not in res:
                                res[profile_node_split[1]] = {}
                            res = res[profile_node_split[1]]
                    else:
                        res[profile_node_split[0]] = {}
                        res = res[profile_node_split[0]]
                        if len(profile_node_split) > 1:
                            if profile_node_split[1] not in res:
                                res[profile_node_split[1]] = {}
                            res = res[profile_node_split[1]]
                        for child in profile_node.children:
                            res[child] = None
                for item in value_dict:
                    res[item] = value_dict[item]

                sql_cmd = "update " + self.__get_root_name() + " set profile=\"" + str(query_res) + \
                    "\" where id=\"" + profile_nodes[0].split(":")[1] + "\""
                if is_android:
                    self.__db.execSQL(sql_cmd)
                else:
                    self.__db.execute(sql_cmd)
                    self.__conn.commit()

                return True
        except BaseException:            # TODO: raise warnings
            return False


if __name__ == "__main__":

    # Create a profile
    profile_hierarchy = ProfileHierarchy('LteRrc')
    root = profile_hierarchy.get_root()
    root.add('Root_leaf', False)
    sib = root.add('Sib', False)
    inter_freq = sib.add('Inter_freq', True)
    inter_freq.add('ThreshXHigh', False)
    inter_freq.add('ThreshXLow', False)
    reconfig = root.add('Reconfig', False)
    drx = reconfig.add('Drx', False)
    drx.add('Drx_short', False)
    drx.add('Drx_long', False)

    profile = Profile(profile_hierarchy)

    res = profile.update(
        'LteRrc:87.Reconfig.Drx', {
            'Drx_short': '1', 'Drx_long': '5'})

    print((profile.query('LteRrc:87.Reconfig.Drx')))

    res = profile.update('LteRrc:87.Reconfig.Drx', {'Drx_long': '6'})

    print((profile.query('LteRrc:87.Reconfig.Drx')))

    print((profile.query('LteRrc:87')))

    res = profile.update(
        'LteRrc:86.Sib.Inter_freq:5780', {
            'ThreshXHigh': '1', 'ThreshXLow': '5'})
    res = profile.update(
        'LteRrc:86.Sib.Inter_freq:1975', {
            'ThreshXHigh': '2', 'ThreshXLow': '8'})

    print((profile.query('LteRrc:86.Sib')))

    profile.update('LteRrc:87', {'Root_leaf': 10})

    print((profile.query('LteRrc:87')))
