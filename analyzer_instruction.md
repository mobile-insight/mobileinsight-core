# Usage of RLC/MAC analyzer

Here I add three new analyzers for RLC/MAC layer:

 * LteRlcRetxAnalyzer
 * LteMacLossCauseAnalyzer
 * LteMacCorruptAnalyzer

The functionality and usage of each analyer are listed below.

## LteRlcRetxAnalyzer

RLC retransmission delay refers to the time interval between packet loss detection (at RLC layer by gap in sequence number) 
and retransmitted packet arriving.

```
overall_delay = delay_to_Nack + delay_of_execute_retx
```

We also record t-reordering timer. Analysis is done for each retransmission of RLC packet. The results are output by log info:

```
[INFO] [LteRlcRetxAnalyzer]: Retransmission. time : 2017-03-07 13:30:01.548979, sequence number : 359, reordering time : 60 (ms), delay to nack : 67 (ms), nack to retransmission : 4 (ms), overall retransmission delay : 71 (ms)
```

## LteMacLossCauseAnalyzer

We attribute unrecoverable packets at MAC to two causes: HARQ failure or loss. This analyzer catches every transmission failure of MAC and 
output the corresponding reason by log info.

```
[INFO] [LteMacLossCauseAnalyzer]: HARQ failure. time: 2017-03-07 13:29:53.363116, packet sequence number (RLC): 34
[INFO] [LteMacLossCauseAnalyzer]: MAC loss. time: 2017-03-07 13:29:59.077922, packet sequence number (RLC): 224
```

## LteMacCorruptAnalyzer

We record the delay of MAC HARQ retransmission for each packet. 

```
[INFO] [LteMacCorruptAnalyzer]: MAC HARQ retransmission delay: 17
[INFO] [LteMacCorruptAnalyzer]: MAC HARQ retransmission delay: 18
[INFO] [LteMacCorruptAnalyzer]: MAC HARQ retransmission delay: 8
```
During analysis, the total number of MAC blocks and the total number of retransmitted packets are recorded for PCell and SCell. 
At the end, the retransmission ratio is calculated as:

```
corruption_ratio_pcell = LteMacCorruptAnalyzerObj.corrupt_blocks['PCell'] / LteMacCorruptAnalyzerObj.total_blocks['PCell']
```
