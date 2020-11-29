/*
PSoC5 Dev Kit PinOuts: 
Dev Kit Part Number: CY8CKIT-059 
http://www.cypress.com/documentation/development-kitsboards/cy8ckit-059-psoc-5lp-prototyping-kit-onboard-programmer-and
http://www.cypress.com/file/157971/download

SysTimer is an additional component from:
http://www.cypress.com/forum/psoc-community-components/systimers-component

FTDI Cable Yellow (USB/UART TX) Port 1.7 (DPM: verified correct)
FTDI Cable Orange (USB/UART RX) Port 1.6
FTDI Cable Black  (USB/UART RX) Ground

*/
/*
#include <project.h>
#include "knobs.h"

#include "MessageHandler.h"
#include "isr_rx_helper.h"
void init();
uint32 SysTicksMS;

uint8 rxbuf[RX_SOFTWARE_BUFFER_LENGTH];
uint8 rxReadIndex  = 0;
uint8 rxWriteIndex = 0;
uint8 rxReadChar = RX_NO_PACKETES; 
float rxReadFloat = 0.0; 
void handleRx();
*/


int main()
{
    /*
    init();   
    uint32 counter = 0;
    while(1)
    {
        if(rxReadIndex != rxWriteIndex)
        {
            parseRxBuffer();
            while( rxReadChar != RX_NO_PACKETES )
            {
                handleRx();
                parseRxBuffer();
            }
        }
        CyDelay(100);
        counter++;
        sendLogMessage("Hello World: %i", counter); 
    }
    */
}
/*
float readAndAverageADC(uint16 numSamples)
{
    CyDelay(1); //Probably unnecessary. 
    
    uint16 ADC_Reads = 0; 
    uint16 sampleCount; 
    float  readVoltage = 0.0;
    for(sampleCount = 0; sampleCount < numSamples; sampleCount++)
    {
        ADC_Reads = ADC_GetResult16();
        readVoltage += ADC_CountsTo_Volts(ADC_Reads);
    }
    readVoltage = readVoltage / ((float) numSamples);
    return readVoltage; 
}
*/
/*
void handleRx()
{
    constructAndSendPacket(MESSAGE_TYPE_FLAG, MESSAGE_FLAG_CHAR_PARSED, 4, (void*)&rxReadChar);
    switch (rxReadChar)
    {
        char buff[16] = {' '};
        sendLogMessage( itoa(rxReadChar,buff,10) ); // send back

        
        //case 'a': //START_FLAG recieved
        //constructAndSendPacket(MESSAGE_TYPE_LOG, MESSAGE_FLAG_NO_FLAG, 4, "aaaa");
        
        
      
            scanStarted = 1;
            waitingForCNC = 1;
            break;
        case 'b': //SCAN_COMPLETE recieved
            scanStarted = 0;
            break;
        case 'm': //CNC ARRIVED 
            waitingForCNC = 0;
            break;
      
    }
}
*/
/*
void init()
{
    
    //CyGlobalIntEnable; 


    ADC_Start();            // Initialize ADC    
    ADC_StartConvert();     // Start conversion    


    //isr_rx_Start();
	//UART_1_Start();     //enable uart
    
   // SysTimers_Start();
 }
*/