# Message Syntax Definition

Survey-box uses a command based logging system to manage communication between the primary micro-controller and the logging controller. This message structure is designed to work over any communication protocol with minimal overhead (in this case, 8 bytes). The message structure is as follows.

Byte #|Value|Definition
------|-----|----------
0 | 0xC3 | Sync Byte Lo
1 | 0xAA | Sync Byte Hi
2 | 0x00 | Message Type (Default Log Message)
3 | n    | Payload Size
4 | ?    | CRC32 Byte 1
5 | ?    | CRC32 Byte 2
6 | ?    | CRC32 Byte 3
7 | ?    | CRC32 Byte 4
8 |      | First Payload Byte
...|     | Payload
8+n|     | Last Payload Byte

## CRC32 Algorithm
The CRC32 library used in this code was cloned from https://github.com/bakercp/CRC32. According to the documentation available at that repo, this implementation is compatible with `java.util.zip.CRC32`.

For reference, the polynomial table used is provided here. In case of any conflicts, refer back to the original source to determine the actual values used.

```C++
static const uint32_t crc32_table[] FLASH_PROGMEM = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};
```

## Message Types And Payloads

### `0x00` Log Data
The payload for this message will be treated as a string and logged directly to the log file.

If data compatibility and precision are concerns, it is recommended that the sender first converts all incoming data to a string of HEX characters, which can then be parsed at a later time.

No response is given to this message.

### `0x01` Append File


### `0x02` New File

### `0xAA` Set Configuration
