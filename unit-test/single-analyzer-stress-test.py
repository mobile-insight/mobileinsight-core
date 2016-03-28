#!/usr/bin/python
# Filename: single-analyzer-stress-test.py

"""
Stress test for each individual analyzer.
For each analyzer, this test traverses all logs in our backend repo, 
and checks the basic robustness.

PLEASE run this script on metro server as follows:
python single-analyzer-stress-test.py 2> stress-test-result.txt

Author: Yuanjie Li
"""
import os
import sys
import logging
import unittest

from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer import Analyzer
import mobile_insight.analyzer

directory="/var/www/mobile_insight/upload/"
# directory="/var/www/mobile_insight/upload/AT&T_samsung-SM-G900T/"


def test_replayer_generator(analyzer_name, log_path):
    """
    Generate a test for a specific operator

    :param analyzer_name: the analyzer to be tested
    :log_path: the path of a mobileinsight log
    """

    def test(self):

        #Single analyzer test only
    	Analyzer.reset()

        src = OfflineReplayer()
        src.set_input_path(log_path)

        #Indirectly call analyzer via include_analyzer
        test_analyzer = Analyzer()
        test_analyzer.include_analyzer(analyzer_name,[])
        test_analyzer.set_source(src)

        # Disable all MobileInsight logs
        # logging.getLogger('mobileinsight_logger').setLevel(logging.CRITICAL)

        src.run()

    return test

class SingleAnalyzerTest(unittest.TestCase):
    """
    A fixture (container) to hold automatically generated tests
    """
    pass

if __name__ == "__main__":


    logs=[]
    for root, d, filenames in os.walk(directory):
        #logs = [f for f in filenames if f.endswith(".qmdl")]
        #break
        for filepath in filenames:
           #logs=logs+[f for f in filepath if f.endswith(".qmdl")] 
           if filepath.endswith(".qmdl") or filepath.endswith("mi2log"):
               logs.append(os.path.join(root,filepath))


    analyzers = mobile_insight.analyzer.__all__
    forbidden_list=["Analyzer","LogAnalyzer","ProfileHierarchy","Profile","StateMachine","HandoffLoopAnalyzer"]
    analyzers = [x for x in analyzers if x not in forbidden_list]

    # analyzers = ["HandoffLoopAnalyzer"]

    test_count = 0


    for analyzer_name in analyzers:
        for log in logs:
            test_name = 'test_%d_%s' % (test_count, analyzer_name)

            sys.stderr.write(test_name+' '+log+'\n')
            test = test_replayer_generator(analyzer_name, log)
            setattr(SingleAnalyzerTest, test_name, test)

            test_count = test_count + 1

    unittest.main()

