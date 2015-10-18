#!/usr/bin/python
# Filename: profile.py
"""
Profile abstraction

Author: Yuanjie Li
"""

from jnius import autoclass

import ast

class ProfileNode(object):
    '''
    A profile node for the ProfileHierarchy tree
    '''
    def __init__(self,name):
        self.name = name
        self.children = {} # A node list to its children


    def add(self,child):
        '''
        Add a child node

        :param child: a child node
        :type child: string

        :returns: the added node
        '''
        child_node = ProfileNode(child)
        self.children[child]=child_node
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
            - Sib1
          - Reconfig
            - MeasConfig
            - Drx
              - Short_drx
              - Long_drx

    The following code constructs such a profile hierachy:

        LteRrcProfile = ProfileHierarchy('LteRrc')
        root = LteRrcProfile.get_root();
        sib=root.add('Sib');
          sib.add('Sib1')
          sib.add('Sib2')
        reconfig=root.add('Reconfig');
          measconfig=reconfig.add('MeasConfig')
          drx=reconfig.add('Drx');
            drx.add('Drx_short');
            drx.add('Drx_long');
    '''

    def __init__(self,root):
        '''
        Initialization of the profile hierarchy

        :param root: the root profile table name
        :type root: string
        '''
        self.__root = ProfileNode(root)

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
            for child in cur_node.children.values():
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
        sql_cmd = 'CREATE TABLE IF NOT EXISTS ' + node.name + "(id,profile,primary key(id))"
        self.__db.execSQL(sql_cmd) 

    def __get_root_name(self):
        return self.__profile_hierarchy.get_root().name

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

    def query(self, init_id, profile_name):
        '''
        Query the profile value with a hierarchical name.
        Example: self.query('cell_id=87','LteRrc.Reconfig.Drx.Short_drx')

        :id: the index in the root (e.g., Physical cell ID)
        :type id: string
        :param profile_name: a hierarcical name separated by '.' (e.g., LteRrc.Reconfig.Drx.Short_drx)
        :type profile_name: string
        :returns: value list that satisfies the query, or None if no such field (id not exist, incomplete record, etc.)
        '''

        #Step 1: check if the name conforms to the hierarchy
        if self.__profile_hierarchy is None: #no profile defined
            return None
        #Check if the field to query is valid
        profile_node = self.__profile_hierarchy.get_node(profile_name)
        if profile_node is None:
            return None

        #Step 2: extract the raw profile
        sql_cmd = "select profile from "+self.__get_root_name()+" where id="+init_id
        sql_res = self.__db.rawQuery(sql_cmd,None)
        sql_res.moveToFirst();
        res = ast.literal_eval(sql_res.getString(0)) #convert string to dictionary
        if not res: #the id does not exist
            return None

        #Step 3: extract the result from raw profile
        profile_nodes = profile_name.split('.')
        for i in range(1,len(profile_nodes)):
            if res is None: #no profile
                break
            res = res[profile_nodes[i]]   
        return res


        

    def update(self, init_id, profile_name, value_dict):
        
        '''
        Update a profile value
        Example: self.update('87','LteRrc.Reconfig.Drx',{Drx_short:1,Drx_long:5})

        If the init_id does not exist, create a new item in the root, with specified values and all other fields as "null" 

        Otherwise, update the specified field values, and keep the ramaining values unchanged. 

        The update operation is atomic. No partial update would be performed

        :param init_id: the initial id in the root
        :type init_id: string
        :param profile_name: a hierarcical name separated by '.' (e.g., LteRrc.Reconfig.Drx)
        :type profile_name: string
        :param value: a field_name->value dictionary of the specified updated values. 
        All the field names should appear in the profile_name.
        :type value: string->string dictionary
        :returns: True if the update succeeds, False otherwise
        '''

        #Step 1: check if the name conforms to the hierarchy
        if self.__profile_hierarchy is None: #no profile defined
            return False
        #Check if the field to update is valid
        test_node = self.__profile_hierarchy.get_node(profile_name)
        if test_node is None:
            return False
        #Check the value fileds to update are indeed included based on hierarchy
        for field_name in value_dict:
            if field_name not in test_node.children:
                #Invalid node
                return False

        #Step 2: check if the id exists or not
        sql_cmd = "select profile from "+self.__get_root_name()+" where id="+init_id
        sql_res = self.__db.rawQuery(sql_cmd,None)
        
        profile_nodes = profile_name.split('.')
        # if not query_res: 
        if sql_res.getColumnCount()==0:
            #The id does not exist. Create a new record

            query_res = {}
            res=query_res
            profile_node = self.__profile_hierarchy.get_root()


            for i in range(1,len(profile_nodes)-1):
                #Initialization
                for child in profile_node.children:
                    res[child]=None
                res[profile_nodes[i]]={}
                res=res[profile_nodes[i]]
                profile_node = profile_node.children[profile_nodes[i]]

            res[profile_nodes[len(profile_nodes)-1]]=value_dict

            #Insert the new record into table
            insert_values = autoclass("android.content.ContentValues")
            insert_values.put("id",init_id)
            insert_values.put("profile","\""+str(query_res)+"\"")
            self.__db.insert(self.__get_root_name(),null,insert_values)
        else:
            sql_res.moveToFirst();
            query_res = ast.literal_eval(sql_res.getString(0)) #convert string to dictionary
            #The id exists. Update the record
            res=query_res
            profile_node = self.__profile_hierarchy.get_root()
            for i in range(1,len(profile_nodes)-1):
                if res[profile_nodes[i]] is not None:
                    res = res[profile_nodes[i]]
                else:
                    res[profile_nodes[i]] = {}
                    res = res[profile_nodes[i]]
                    for child in profile_node.children:
                        res[child]=None
            for item in value_dict:
                res[item]=value_dict[item]
            update_values = autoclass("android.content.ContentValues")
            update_values.put("profile","\""+str(query_res)+"\"")
            self.__db.update(self.__get_root_name(),update_values,"id="+init_id,None)

if __name__=="__main__":

    #Create a profile
    profile_hierarchy=ProfileHierarchy('LteRrc');
    root=profile_hierarchy.get_root()
    sib=root.add('Sib');
    reconfig=root.add('Reconfig');
    drx=reconfig.add('Drx');
    drx.add('Drx_short');
    drx.add('Drx_long');

    profile=Profile(profile_hierarchy)

    res = profile.update("87",'LteRrc.Reconfig.Drx',{'Drx_short':'1','Drx_long':'5'})
    
    print profile.query("87",'LteRrc.Reconfig.Drx')


