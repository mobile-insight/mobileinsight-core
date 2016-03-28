The purpose of the analyzer test is to guarantee the '''correctness and robustness''' for each MobileInsight analyzer. 


===Methodology===

We apply '''unit test''' to each analyzer by '''replaying logs'''. The unit test is performed on laptop rather than on the phone, because the analyzer code is platform-independent (P4A does not support unittest module). Throughout these tests, we use QmdlReplayer to replay representative offline logs from different network operators (AT&T, T-Mobile, Verizon, Sprint, China Mobile, etc.) and 3GPP releases. We assume the QmdlReplayer functions correctly. 

In general, the tests can be classified into two categories: '''single-analyzer''' and '''multi-analyzer''' test. In single analyzer test, each time only 1 analyzer is tested. In multi-analyzer test (NOT DONE), we test the composition of different analyzers, and check wether there are any conflicts.

All test codes are written in Python with its unittest framework. All the codes should be placed in mobile_insight/test. To run all the tests:
  cd mobile_insight/test/
  python YOUR_TEST.py

===Single-Analyzer Test===

There are two types of tests: '''generic test''' and '''analyzer-specific test'''. The generic test is specific to all analyzers, and only checks basic function correctness and robustness. Instead, the analyzer-specific test is written on per-analyzer granularity. It checks specific function correctness for each analyzer. Both tests should be performed over different network operators and 3GPP releases.

The generic test code is located in mobile_insight/test/single-analyzer-generic-test.py . It enumerates all available analyzers, and runs each individual analyzer under different network operators (test logs are located in mobile_insight/test/test-logs/, more logs are needed). The test fails if any exceptions/errors are raised, or the analyzers cannot be successfully loaded. The traceback logs will be provided for each test failure. 



'''TODO:''' add analyzer-specific tests


===Multi-Analyzer Test===

'''TODO:''' specify the goal and methodology to test analyzer interplays.
