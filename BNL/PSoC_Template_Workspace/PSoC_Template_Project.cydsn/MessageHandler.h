
#ifndef MESSAGE_HANDLER_H
    #define MESSAGE_HANDLER_H
    
    #include "knobs.h"
    #include <cytypes.h>
    #include "SysTimers.h"
    #include "stdio.h"
    #include "stdlib.h"
    #include "CyLib.h"
    #include "UART_1.h"
    
    #define UART_PACKET_DELAY_MS 1 // small delay before sending packet to not overwhelm python software

    #define RX_NEXT_IS_CHAR               0x05
    #define RX_NEXT_IS_FLAG_AND_FLOAT     0x06
    #define RX_FLAG_SET_MODE_1            0xc8 //200
    #define RX_FLAG_SET_MODE_2            0xc9 //201
    #define RX_FLAG_SET_MODE_3            0xca //202
    
    #define RX_NO_PACKETES               (uint8)0x0

    //Message Type Definitions
    #define MESSAGE_TYPE_LOG            (uint8)1
    //#define MESSAGE_TYPE_ASCII_DATA   (uint8)2 //Todo: implement in Python? Probably not needed
    #define MESSAGE_TYPE_BINARY_FLOAT   (uint8)3
    #define MESSAGE_TYPE_FLAG           (uint8)4
    
    //Message Flags to Indicate State Change or Alert

    #define MESSAGE_FLAG_TIMESTAMP              (uint8)99
    #define MESSAGE_FLAG_MOVE_TO_NEXT_POSITION  (uint8)15    
    #define MESSAGE_FLAG_NO_FLAG                (uint8)11
    #define MESSAGE_FLAG_CHAR_RECIEVED          (uint8)160
    #define MESSAGE_FLAG_CHAR_PARSED            (uint8)161
    #define MESSAGE_FLAG_ALIGNMENT_SENSORS      (uint8)100
    #define MESSAGE_FLAG_PGA_SETTINGS           (uint8)101

    #define UPDATE_TIMESTAMP_FOR_EACH_PACKET 1

    void sendLogMessage(const char* format, ...);
    void constructHeader(uint8 messageType, uint8 messageFlag, uint16 payloadBytes, void* payLoad);
    void sendPacket();
    void constructAndSendPacket(uint8 messageType, uint8 messageFlag, uint16 payloadBytes, void* payLoad);
#endif