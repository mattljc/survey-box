# Message Syntax Definition

Survey-box uses a command based logging system to manage communication between the primary micro-controller and the logging controller. This message structure is designed to work over any communication protocol with minimal overhead (in this case, 8 bytes). The message structure is as follows.

Byte #|Value|Definition
-----------------------
0 | 0xC3 | Sync Byte Lo
1 | 0xAA | Sync Byte Hi
2 | n    | Message Type
3 |      | Payload Size
4 |      | CRC32 Byte 1
5 |      | CRC32 Byte 2
6 |      | CRC32 Byte 3
7 |      | CRC32 Byte 4
8 |      | First Payload Byte
...|     | Payload
8+n|     | Last Payload Byte
