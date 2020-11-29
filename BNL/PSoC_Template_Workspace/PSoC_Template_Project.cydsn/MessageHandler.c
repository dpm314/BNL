/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */

#include "MessageHandler.h"
//#include <device.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define MAGIC_HEAD_NUMBER (uint8)165 //165 = 0xA5 in hex
#define MAGIC_TAIL_NUMBER (uint8)182 //182 = 0xB6 in hex

//define private functions here
static void _sendHeader();
static void _sendPayload();
static void _sendTail();
static void _sendHeadMagicNumber();
static void _sendTailMagicNumber();
    
static uint8  _headerBuff[4] = {0};
static uint8  _tailBuff[4]   = {0};
static uint8  _timestampBuff[4] = {0};
static uint8*  _payload; 
static uint16 _numBytes; 
static uint8  _messageType;
static uint8  _messageFlag;

extern uint32 SysTicksMS; 

void constructHeader(uint8 messageType, uint8 messageFlag, uint16 payloadBytes, void* payload)
{
    uint8* bytePtr = (uint8*) &payloadBytes;
    //Populate Header Buffer
    
    _headerBuff[0] = messageType; 
    _headerBuff[1] = messageFlag;
    
    //pack payloadBytes (lenght of payload in bytes)
    _headerBuff[2] = *bytePtr++;
    _headerBuff[3] = *bytePtr;
    
    //update global timestamp
    #if(UPDATE_TIMESTAMP_FOR_EACH_PACKET)
        //SysTicksMS = SysTimers_GetSysTickValue();
        SysTicksMS = 1; 
    #endif 
    //pack timestamp 
    bytePtr = (uint8*)&SysTicksMS; 
    _timestampBuff[0] = *bytePtr++;
    _timestampBuff[1] = *bytePtr++;
    _timestampBuff[2] = *bytePtr++;
    _timestampBuff[3] = *bytePtr;
    
    //record message descriptors
    _messageType = messageType; 
    _messageFlag = messageFlag;   
    _numBytes = payloadBytes;
}

static void _sendHeader()
{
    UART_1_PutChar( _headerBuff[0] ); 
    UART_1_PutChar( _headerBuff[1] ); 
    UART_1_PutChar( _headerBuff[2] ); 
    UART_1_PutChar( _headerBuff[3] ); 
    
    UART_1_PutChar( _timestampBuff[0] ); 
    UART_1_PutChar( _timestampBuff[1] ); 
    UART_1_PutChar( _timestampBuff[2] ); 
    UART_1_PutChar( _timestampBuff[3] );
}

static void _sendPayload()
{
       
    uint8* bytePty = (uint8*) _payload;
    uint16 byteCounter;    
   
    for(byteCounter = 0; byteCounter < _numBytes; byteCounter++)
    {
        UART_1_PutChar( *bytePty++ );
    }
}

static void _sendTail()
{     
    //Todo: add meaningful tail. will need to adjust tailsize in Packet.__init__()
    
    /*
    UART_1_PutChar( _tailBuff[0] );
    UART_1_PutChar( _tailBuff[1] );
    UART_1_PutChar( _tailBuff[2] );
    UART_1_PutChar( _tailBuff[3] );   
    */
    _sendTailMagicNumber();
}

static void _sendHeadMagicNumber()
{        
    UART_1_PutChar( MAGIC_HEAD_NUMBER );
    UART_1_PutChar( MAGIC_HEAD_NUMBER );
    UART_1_PutChar( MAGIC_HEAD_NUMBER );
    UART_1_PutChar( MAGIC_HEAD_NUMBER );
}   

static void _sendTailMagicNumber()
{
    UART_1_PutChar( MAGIC_TAIL_NUMBER );
    UART_1_PutChar( MAGIC_TAIL_NUMBER );
    UART_1_PutChar( MAGIC_TAIL_NUMBER );
    UART_1_PutChar( MAGIC_TAIL_NUMBER );
}

void sendPacket()
{
    //sends 4 byte header to UART
    //only if message is not a flag (ie it is log message, binary or float data) sends payload
    
#if (1 == ENABLE_RATTLESNAKE_COMMUNICATION)
    CyDelayUs( UART_PACKET_DELAY_MS ); // short delay to throttle absolute maximum number of packets sent per second
    _sendHeadMagicNumber();
    _sendHeader();
    if( _messageType != MESSAGE_TYPE_FLAG )
    {
        _sendPayload();
    }
    _sendTail();  
#endif
}

void constructTail()
{
    uint8* bytePtr = (uint8*)&SysTicksMS;
    _tailBuff[0] = *bytePtr++;
    _tailBuff[1] = *bytePtr++;
    _tailBuff[2] = *bytePtr++;
    _tailBuff[3] = *bytePtr;
}

void constructAndSendPacket(uint8 messageType, uint8 messageFlag, uint16 payloadBytes, void* thePayload)
{
    constructHeader(messageType,messageFlag,payloadBytes,thePayload);
    _payload  = (uint8*) thePayload;
    constructTail(); 
    sendPacket();
    CyDelay(UART_PACKET_DELAY_MS); // throttles maximum speed that packets individual packets can be dumped
}
    

void sendLogMessage(const char* format, ...)
{
    va_list args; 
    size_t formatLen;
    char messageBuf[256];
    va_start(args, format);
    formatLen = vsnprintf(messageBuf, 256, format, args);
    va_end(args); 
    constructAndSendPacket(MESSAGE_TYPE_LOG, MESSAGE_FLAG_NO_FLAG, formatLen, (void*)messageBuf);    
}