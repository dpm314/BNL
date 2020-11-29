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


#ifndef ISR_RX_HELPER
    #define ISR_RX_HELPER
    
    #define RX_SOFTWARE_BUFFER_LENGTH (uint8) 32
    #include <cytypes.h>
    //#include <device.h>
    uint8 _getBytesAvailable();
    uint8  parseChar();
    float parseFloat();
    
    void parseRxBuffer();
#endif