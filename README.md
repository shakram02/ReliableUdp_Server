# ReliableUdp_Server
A multi threadded server side C++ application that transferes files to clients using Enhanced UDP

### Enhanced UDP ?
The program implements GBN ( Go back N ), packets are sent to the client, the client either ACKs the packet
or the packets reach a send treshold, then transmission is aborted.

** This project will not compile with using the library that contains the core classes, it's also here on GitHub**
