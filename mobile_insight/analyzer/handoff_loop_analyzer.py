#!/usr/bin/python
# Filename: handoff_loop_analyzer.py
"""
An analyzer for handoff persistent loop detection

Author: Yuanjie Li
"""
import xml.etree.ElementTree as ET
from .analyzer import *
from .rrc_analyzer import RrcAnalyzer


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
        self.include_analyzer("RrcAnalyzer", [self.callback_loop_detection])

    def find_loops(self):
        """
        Detection persistent loops caused by handoff policy conflicts.
        The results would be written into the log.
        """
        self.__loop_detection()

    def callback_loop_detection(self, msg):
        """
        Detection persistent loops caused by handoff policy conflicts.
        The results would be written into the log.

        :param msg: the event from RrcAnalyzer
        """

        if msg.type_id != "RrcAnalyzer":
            return

        pass  # TODO: periodically find loops

    def __loop_detection(self):

        # Get cell list and configurations
        # cell_list = self.__rrc_analyzer.get_cell_list()
        cell_list = self.get_analyzer("RrcAnalyzer").get_cell_list()
        # ignore unknown cells
        cell_list = [x for x in cell_list if (x[0] and x[1])]
        # mark if a cell has been visited:
        cell_visited = {x: False for x in cell_list}

        self.log_warning("total cells: " +
                         str(len(cell_list)) + " " + str(cell_list))

        # print cell_list
        # for cell in cell_list:
        #     # self.__rrc_analyzer.get_cell_config(cell).dump()
        #     # self.log_info(self.__rrc_analyzer.get_cell_config(cell).dump())
        #     self.log_info(self.get_analyzer("RrcAnalyzer").get_cell_config(cell).dump())

        # each cell's configuration
        cell_config = {x: self.get_analyzer(
            "RrcAnalyzer").get_cell_config(x) for x in cell_list}
        # print cell_config

        if cell_list:

            # We implement the loop detection algorithm in Proposition 3,
            # because preferences are observed to be inconsistent

            # while False in cell_visited.itervalues():
            while False in list(cell_visited.values()):
                # some cells have not been explored yet

                # In each round, we report loops with *unvisited_cell* involved
                unvisited_list = [
                    x for x in cell_visited if not cell_visited[x]]
                unvisited_cell = unvisited_list[0]
                self.log_debug("unvisited_cell " + str(unvisited_cell))

                # neighbor_cells = self.__rrc_analyzer.get_cell_neighbor(unvisited_cell)
                neighbor_cells = self.get_analyzer(
                    "RrcAnalyzer").get_cell_neighbor(unvisited_cell)
                # Rule out visited cells
                neighbor_cells = [
                    x for x in neighbor_cells if not cell_visited[x]]

                # Neighboring cell vistied list
                # For each cell: 0 for unvisited, 1 for idle-only visited, 2
                # for idle-active visited
                neighbor_visited = {x: False for x in neighbor_cells}

                # DFS stacks for loop detection
                dfs_stack = [unvisited_cell]  # cells involved in handoff chain
                # each cell in the handoff chain has a {neighbor->visited} dict
                neighbor_stack = [neighbor_visited]
                # Symolic stack: sym_stack[i] means who is still on the list
                #-1 if -ri, 1 if ri
                sym_list_stack = [[0]]
                # latent variable to detect loop
                val_stack = [0]

                while dfs_stack:
                    # self.log_warning("dfs_stack:"+str(dfs_stack))
                    # print "unvisited_cell",neighbor_stack[0]
                    # print "dfs_stack",dfs_stack
                    if len(neighbor_stack) >= 1:
                        self.log_debug(
                            "neighbor_stack[-1] " + str(dfs_stack[-1]) + " " + str(neighbor_stack[-1]))
                    src_cell = dfs_stack.pop()
                    src_neighbor = neighbor_stack.pop()
                    src_sym_list = sym_list_stack.pop()
                    src_val = val_stack.pop()

                    dst_cell = None
                    dst_sym_list = None
                    dst_val = None

                    # Find a next cell to handoff
                    for cell in src_neighbor:
                        if not src_neighbor[cell]:
                            # Unvisited neighbor
                            if not dfs_stack or cell not in dfs_stack[1:]:
                                dst_cell = cell
                                # Now dst_cell has been visited by src_cell
                                src_neighbor[cell] = True
                                break
                            else:
                                # intermediate cells duplicate
                                # ignore it, and mark it as visited
                                src_neighbor[cell] = True

                    if not dst_cell:
                        # src_cell's all neighbors have been visited
                        continue

                    # src_neighbor[dst_cell]=True #Now dst_cell has been
                    # visited by src_cell
                    self.log_debug("dst_cell:" + str(dst_cell)
                                   + " state:" + str(src_neighbor[dst_cell]))

                    # Step 1: IDLE-STATE HANDOFF, src_cell->dst_cell
                    src_freq = cell_config[src_cell].status.freq
                    dst_freq = cell_config[dst_cell].status.freq
                    dst_config = cell_config[src_cell].get_cell_reselection_config(
                        cell_config[dst_cell].status)

                    if not dst_config:
                        # src_cell cannot reach dst_cell: no handoff config
                        # available
                        self.log_warning(
                            "no handoff config: " +
                            str(src_cell) +
                            "->" +
                            str(dst_cell))
                        dfs_stack.append(src_cell)
                        neighbor_stack.append(src_neighbor)
                        sym_stack.append(src_sym)
                        val_stack.append(src_val)
                        continue

                    dst_neighbor_cells = self.get_analyzer(
                        "RrcAnalyzer").get_cell_neighbor(dst_cell)
                    # Rule out visited cells
                    dst_neighbor_cells = [
                        x for x in dst_neighbor_cells if not cell_visited[x]]
                    # Rule out cells that have been in the stack (except first one)
                    # Otherwise subloops without cell_visited may be triggered
                    if dfs_stack:
                        dst_neighbor_cells = [
                            x for x in dst_neighbor_cells if x not in dfs_stack[1:]]

                    # Neighboring cell vistied list
                    # For each cell: 0 for unvisited, 1 for idle-only visited,
                    # 2 for idle-active visited
                    dst_neighbor = {x: False for x in dst_neighbor_cells}

                    # Extract src_cell and dst_cell's preference relation
                    src_pref = cell_config[src_cell].sib.serv_config.priority
                    dst_pref = dst_config.priority
                    if not src_pref or not dst_pref:  # happens in 3G
                        # 25.331: without pref, treat as equal pref
                        src_pref = None
                        dst_pref = None

                    dst_sym_list = src_sym_list

                    # Push back src_cell
                    dfs_stack.append(src_cell)
                    neighbor_stack.append(src_neighbor)
                    sym_list_stack.append(src_sym_list)
                    val_stack.append(src_val)

                    if src_pref < dst_pref:
                        dst_sym_list.append(1)
                        dst_val = dst_config.threshx_high
                    elif src_pref > dst_pref:

                        # Step 1: test if ri<threshserv_low can happen in this
                        # case
                        total_val_right_side = sum(val_stack)
                        if total_val_right_side > dst_config.threshserv_low:
                            continue

                        cur_src = dst_sym_list.pop() - 1
                        dst_sym_list.append(cur_src)
                        dst_sym_list.append(1)
                        dst_val = dst_config.threshx_low - dst_config.threshserv_low

                    elif src_freq == dst_freq or src_pref == dst_pref:
                        # BUG HERE!!!! EQUAL-PREF HANDOFF NOT WELL HANDLED!
                        # CAUSED BY NUMERICAL UPDATE
                        cur_src = dst_sym_list.pop() - 1
                        dst_sym_list.append(cur_src)
                        dst_sym_list.append(1)

                        if dst_config.offset:
                            dst_val = dst_config.offset
                        else:  # trace not ready
                            # continue
                            dst_val = 0

                    if dst_cell == dfs_stack[0]:
                        # Loop may occur

                        dst_sym_list.pop()
                        dst_sym_list[0] = dst_sym_list[0] + 1
                        total_val_right_side = sum(val_stack) + dst_val

                        loop_exist = False
                        if 1 in dst_sym_list:
                            # positive symbol (ri) exists.
                            # In this case, loop can always happen with proper values,
                            # no matter whether right side is positive or not
                            loop_exist = True
                        elif -1 in dst_sym_list:
                            # no positive symbol, but neg sym (-ri) exists
                            if total_val_right_side <= 0:
                                loop_exist = True
                        else:
                            # no symbols on left side
                            if total_val_right_side <= 0:
                                loop_exist = True

                        if loop_exist:
                            print(dst_sym_list)  # BUG: dst_sym_list much longer than loop
                            loop_report = "\033[31m\033[1mPersistent loop: \033[0m\033[0m"
                            loop_report += str(dfs_stack[0])
                            prev_item = dfs_stack[0]
                            for item in range(1, len(dfs_stack)):
                                cell = dfs_stack[item]
                                if neighbor_stack[item - 1][cell] == 1:
                                    loop_report += "(idle)->" + str(cell)
                                elif neighbor_stack[item - 1][cell] == 2:
                                    loop_report += "(active)->" + str(cell)

                            if src_neighbor[dst_cell] == 1:
                                loop_report += "(idle)->" + str(dst_cell)
                            elif src_neighbor[dst_cell] == 2:
                                loop_report += "(active)->" + str(dst_cell)

                            self.log_warning(loop_report)
                            # raw_input("Debugging...")
                            continue

                    else:
                        # No loop found, push dst_cell into stack
                        dfs_stack.append(dst_cell)
                        neighbor_stack.append(dst_neighbor)
                        sym_list_stack.append(dst_sym_list)
                        val_stack.append(dst_val)

                cell_visited[unvisited_cell] = True
