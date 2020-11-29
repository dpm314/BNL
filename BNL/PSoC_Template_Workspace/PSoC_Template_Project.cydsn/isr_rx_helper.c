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

//#include "knobs.h"
//#include <device.h>
#include "MessageHandler.h"
#include "isr_rx_helper.h"
extern uint8 rxbuf[RX_SOFTWARE_BUFFER_LENGTH];
extern uint8 rxReadIndex;
extern uint8 rxWriteIndex;
extern uint8 rxReadChar; 
extern float rxReadFloat; 
uint8 _getBytesAvailable()
{
    uint8 step = 0;
    uint8 numBytesAvailable = 0; 
    if( rxReadIndex < rxWriteIndex ) {step = RX_SOFTWARE_BUFFER_LENGTH; }
    numBytesAvailable = rxWriteIndex + step - rxReadIndex;
    return numBytesAvailable; 
}

void parseRxBuffer()
{
    //Determine how many bytes available
    //uint8 numBytesAvailable = 0;
    rxReadChar = RX_NO_PACKETES; 
    rxReadFloat = 0.0;

    if( rxWriteIndex != rxReadIndex )
    {
        rxReadIndex++;
        
        if( rxbuf[rxReadIndex-1] == RX_NEXT_IS_CHAR )
        {
            rxbuf[rxReadIndex-1] = 0x0;
            while(_getBytesAvailable() < 1)
            {
                CyDelayUs(10);
                sendLogMessage("parseRxBuffer() RX_NEXT_IS_CHAR: waiting for _getBytesAvailable() < 1");
            }
            rxReadChar = parseChar();
        }
        else if( rxbuf[rxReadIndex-1] == RX_NEXT_IS_FLAG_AND_FLOAT )
        {
            rxbuf[rxReadIndex-1] = 0x0;
            while(_getBytesAvailable() < 5)
            {
                CyDelayUs(10);
                sendLogMessage("parseRxBuffer() RX_NEXT_IS_FLAG_AND_FLOAT: waiting for _getBytesAvailable() < 5");
            }
            rxReadChar  = parseChar();  //increments rxReadIndex by 1
            rxReadFloat = parseFloat(); //increments rxReadIndex by 4
        }
    }
}
uint8 parseChar()
{
    // rxReadIndex Points AT location of char
    uint8 rxChar;
    rxChar = rxbuf[rxReadIndex];
    //rxbuf[rxReadIndex] = 0x0; 
    rxReadIndex++; 
    rxReadIndex %= RX_SOFTWARE_BUFFER_LENGTH;
    return rxChar;
}

float parseFloat()
{
    // rxReadIndex Points AT start of float
    float aFloat; 
    uint8* dataPtr = (uint8*)&aFloat;
    
    uint8 byteCounter = 0; 
    for(byteCounter = 0; byteCounter < 4; byteCounter++)
    {
        *dataPtr++ = rxbuf[rxReadIndex];          //read first byte of float
        //rxbuf[rxReadIndex] = 0x0; 
        rxReadIndex++;
        rxReadIndex %=RX_SOFTWARE_BUFFER_LENGTH;
    }
    
    return aFloat;
}