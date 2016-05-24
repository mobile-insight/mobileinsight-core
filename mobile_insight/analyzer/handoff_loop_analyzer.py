#!/usr/bin/python
# Filename: handoff_loop_analyzer.py
"""
An analyzer for handoff persistent loop detection

Author: Yuanjie Li
"""
try: 
    import xml.etree.cElementTree as ET 
except ImportError: 
    import xml.etree.ElementTree as ET
from analyzer import *
from rrc_analyzer import RrcAnalyzer

class HandoffLoopAnalyzer(Analyzer):
    """
    An analyzer for handoff persistent loop detection. 
    It relies on RrcAnalyzer to extract idle/active-state handoff policies.
    """

    def __init__(self):
        """
        Initialization of the analyzer.
        """
        Analyzer.__init__(self)
        # self.include_analyzer("RrcAnalyzer",[self.__loop_detection])
        self.include_analyzer("RrcAnalyzer",[self.callback_loop_detection])

    def find_loops(self):
    	"""
    	Detection persistent loops caused by handoff policy conflicts.
        The results would be written into the log. 
    	"""
    	self.__loop_detection()

    def callback_loop_detection(self,msg):
    	"""
        Detection persistent loops caused by handoff policy conflicts.
        The results would be written into the log. 

        :param msg: the event from RrcAnalyzer
        """

    	if msg.type_id!="RrcAnalyzer":
            return
            
        pass #TODO: periodically find loops


    def __loop_detection(self):

        #Get cell list and configurations
        # cell_list = self.__rrc_analyzer.get_cell_list()
        cell_list = self.get_analyzer("RrcAnalyzer").get_cell_list()
        # ignore unknown cells
        cell_list = [x for x in cell_list if (x[0] and x[1])]
        print len(cell_list)
        # mark if a cell has been visited:
        cell_visited = {x:False for x in cell_list} 
        
        # print cell_list
        # for cell in cell_list:
        #     # self.__rrc_analyzer.get_cell_config(cell).dump()
        #     # self.log_info(self.__rrc_analyzer.get_cell_config(cell).dump())
        #     self.log_info(self.get_analyzer("RrcAnalyzer").get_cell_config(cell).dump())

        # each cell's configuration
        cell_config = {x:self.get_analyzer("RrcAnalyzer").get_cell_config(x) for x in cell_list}
        # print cell_config
        
        if cell_list:

            # We implement the loop detection algorithm in Proposition 3,
            # because preferences are observed to be inconsistent

            # while False in cell_visited.itervalues():    
            while False in cell_visited.values():
                # some cells have not been explored yet    

                # In each round, we report loops with *unvisited_cell* involved     
                unvisited_list = [x for x in cell_visited if not cell_visited[x]]
                unvisited_cell = unvisited_list[0]
                print "unvisited_cell", unvisited_cell

                # neighbor_cells = self.__rrc_analyzer.get_cell_neighbor(unvisited_cell)
                neighbor_cells = self.get_analyzer("RrcAnalyzer").get_cell_neighbor(unvisited_cell)
                # Rule out visited cells
                neighbor_cells=[x for x in neighbor_cells if not cell_visited[x]]

                # Neighboring cell vistied list
                # For each cell: 0 for unvisited, 1 for idle-only visited, 2 for idle-active visited
                neighbor_visited = {x: False for x in neighbor_cells}

                
                # DFS stacks for loop detection
                dfs_stack=[unvisited_cell]    #cells involved in handoff chain
                neighbor_stack=[neighbor_visited] #each cell in the handoff chain has a {neighbor->visited} dict
                #dcell_clear_stack[i]=False if ci-1->ci is "high-pref" handoff, true otherwise
                cell_clear_stack=[True]
                val_stack=[0]   #latent variable to detect loop

                while dfs_stack:
                    self.log_debug("dfs_stack:"+str(dfs_stack))
                    # print "unvisited_cell",neighbor_stack[0]
                    # print "dfs_stack",dfs_stack
                    if len(neighbor_stack)>=1:
                        print "dfs_stack",dfs_stack
                        print "\033[91m\033[1mneighbor_stack[-1]\033[0m\033[0m",dfs_stack[-1],neighbor_stack[-1]
                    src_cell = dfs_stack.pop()
                    src_neighbor = neighbor_stack.pop()
                    src_clear = cell_clear_stack.pop()
                    src_val = val_stack.pop()
                    dst_cell = None
                    dst_clear = None
                    dst_val = None
                    
                    #Find a next cell to handoff
                    for cell in src_neighbor:
                        if not src_neighbor[cell]:
                            #Unvisited neighbor
                            if not dfs_stack or cell not in dfs_stack[1:]:
                                dst_cell = cell
                                #Now dst_cell has been visited by src_cell
                                src_neighbor[cell] = True
                                break
                            else:
                                #intermediate cells duplicate
                                #ignore it, and mark it as visited
                                src_neighbor[cell] = True
        
                    if not dst_cell:
                        # src_cell's all neighbors have been visited
                        continue

                    # src_neighbor[dst_cell]=True #Now dst_cell has been visited by src_cell
                    self.log_debug("dst_cell:"+str(dst_cell)\
                        +" state:"+str(src_neighbor[dst_cell]))

                    # Step 1: IDLE-STATE HANDOFF, src_cell->dst_cell
                    src_freq=cell_config[src_cell].status.freq
                    dst_freq=cell_config[dst_cell].status.freq
                    dst_config=cell_config[src_cell].get_cell_reselection_config(cell_config[dst_cell].status)
                    
                    if not dst_config:
                        # src_cell cannot reach dst_cell: no handoff config available
                        print "no dst_config",src_cell,dst_cell
                        dfs_stack.append(src_cell)
                        neighbor_stack.append(src_neighbor)
                        cell_clear_stack.append(src_clear)
                        val_stack.append(src_val)
                        continue
                    
                    dst_neighbor_cells=self.get_analyzer("RrcAnalyzer").get_cell_neighbor(dst_cell)
                    # Rule out visited cells
                    dst_neighbor_cells=[x for x in dst_neighbor_cells if not cell_visited[x]]
                    # Rule out cells that have been in the stack (except first one)
                    # Otherwise subloops without cell_visited may be triggered
                    if dfs_stack:
                        dst_neighbor_cells=[x for x in dst_neighbor_cells if x not in dfs_stack[1:]]

                    # Neighboring cell vistied list
                    # For each cell: 0 for unvisited, 1 for idle-only visited, 2 for idle-active visited
                    dst_neighbor={x: False for x in dst_neighbor_cells}

                    dfs_stack.append(src_cell)
                    neighbor_stack.append(src_neighbor)
                    cell_clear_stack.append(src_clear)
                    val_stack.append(src_val)

                    # Extract src_cell and dst_cell's preference relation
                    src_pref=cell_config[src_cell].sib.serv_config.priority
                    dst_pref=dst_config.priority
                    if not src_pref or not dst_pref:    #happens in 3G
                        #25.331: without pref, treat as equal pref
                        src_pref = None
                        dst_pref = None

                    if src_freq==dst_freq or src_pref==dst_pref:
                        dst_clear = True
                        if dst_config.offset:
                            dst_val = src_val+dst_config.offset
                        else: #trace not ready
                            # continue
                            dst_val = src_val

                    elif src_pref<dst_pref:
                        dst_clear = False
                        dst_val = dst_config.threshx_high
                    elif src_pref>dst_pref:
                        if src_val>dst_config.threshserv_low:
                            # Pruning happens: no loop can be triggered
                            continue
                        dst_clear = True
                        dst_val = src_val+dst_config.threshx_low-dst_config.threshserv_low
                        # dst_val = dst_config.threshx_low
                        
                    if dst_cell == dfs_stack[0]:
                        #Loop may occur
                        #Special check: if dfs_stack[0] performs low-pref handoff
                        dst_dst_cell = dfs_stack[1]
                        
                        dst_dst_config=cell_config[dst_cell].get_cell_reselection_config(cell_config[dst_dst_cell].status)
                        

                        dst_serv_pref = cell_config[dst_cell].sib.serv_config.priority
                        dst_dst_pref = dst_dst_config.priority

                        if not dst_serv_pref or not dst_dst_pref:    #happens in 3G
                            #25.331: without pref, treat as equal pref
                            dst_serv_pref = dst_dst_pref = None

                        if dst_serv_pref>dst_dst_pref \
                        and dst_val>dst_dst_config.threshserv_low:
                            continue

                        if False in cell_clear_stack or dst_val<0:
                            # loop_report="\033[31m\033[1mPersistent loop: \033[0m\033[0m"
                            loop_report+=str(dfs_stack[0])
                            prev_item=dfs_stack[0]
                            for item in range(1,len(dfs_stack)):
                                cell=dfs_stack[item]
                                if neighbor_stack[item-1][cell]==1:
                                    loop_report+="(idle)->"+str(cell)
                                elif neighbor_stack[item-1][cell]==2:
                                    loop_report+="(active)->"+str(cell)

                            if src_neighbor[dst_cell]==1:
                                loop_report+="(idle)->"+str(dst_cell)
                            elif src_neighbor[dst_cell]==2:
                                loop_report+="(active)->"+str(dst_cell)

                            self.log_warning(loop_report)
                            raw_input("Debugging...")
                            continue

                    dfs_stack.append(dst_cell)
                    neighbor_stack.append(dst_neighbor)
                    cell_clear_stack.append(dst_clear)
                    val_stack.append(dst_val)

                print "here?"
                cell_visited[unvisited_cell]=True



