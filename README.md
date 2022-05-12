# Reliable-Transport-Protocols
The three reliable data transport protocols: Alternating-Bit (ABT), Go-Back-N (GBN), and Selective-Repeat (SR), have been implemented in the given simulator, and the performance of the three has been compared with different loss and corruption probabilities.

Over all the experiments, SR generally outperforms GBN, and GBN generally outperforms ABT, with respect to throughput, as is expected. There was no discernible trend in the average time taken for the protocols to send messages, and this could be attributed to the difference in the timeout values.
