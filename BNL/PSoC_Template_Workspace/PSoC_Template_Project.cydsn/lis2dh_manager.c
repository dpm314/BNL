/******************************************************************************
 *
 * @file   lis3md_manager.c
 * @author toby_lou & david meichle
 * @date   03-aug-2016 2:54:38 PM
 *
 * @brief
 *
 * @copyright
 * WiTricity Corporation
 * CONFIDENTIAL AND PROPRIETARY
 * Copyright 2016 WiTricity Corporation
 * All Rights Reserved
 *
 * NOTICE:  All information contained herein is and remains the property
 * of WiTricity Corporation and its licensors.  The intellectual and
 * technical concepts contained herein are proprietary to WiTricity and
 * may be covered by U.S. and foreign patents and patent applications,
 * and are protected by trade secret and copyright law.  Dissemination of
 * this information or reproduction of this material is strictly
 * forbidden unless prior written permission is obtained from WiTricity.
 *
 *****************************************************************************/
#include <project.h>
#include "lis2dh_manager.h"
#include "i2c_service.h"
#include <stdio.h>

/******************************************************************************
 ******************************************************************************
 * PRIVATE DATA
 ******************************************************************************
 ******************************************************************************/


/******************************************************************************
 ******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************
 ******************************************************************************/

void lis2dh_init()
{
    /*
    // 0x70 = 0b01110000
    // OM = 11 (ultra-high-performance mode for X and Y); DO = 100 (10 Hz ODR)
        i2cWriteReg(device, LIS2DH_CTRL_REG1, 0x70);
    */
    // 0x62 = 0b01100010
    // OM = 11; DO = xxx = 000; FAST_ODR = 1; Should be 155 Hz MAX ODR
    
    //see Pg 5/23 of Datasheet for Register Definition
    
         
    // 0x00 = 0b00000000
    // MD = 00 (continuous-conversion mode)
    i2cWriteReg( LIS2DH_CTRL_REG3, 0x00);  // for (fast) mode/ODR
    
    // 0x00 = 0b00000000
    // FS = 00 (+/- 4 gauss full scale)
    i2cWriteReg( LIS2DH_CTRL_REG2, 0x00);
   
    //0x40 = 0b01000000 // Block Data Update (see page 14/23 section 3.2.3)
    // This is absolutely necessary, verified in no field data that omititng this will cause frequent corruption of data
    i2cWriteReg( LIS2DH_CTRL_REG5, 0x40);   
            
    CyDelay(50); //Wait 50 msec for LIS2DH to settle. ToDo: find exact value settle time, this is a guess
}

/******************************************************************************
 *
 * lis2dh_getAccelerationOutputs
 *
 ******************************************************************************/ 
void lis2dh_getAccelerationOutputs( float accelerations[3])
{
    
 /*   while( ! (i2cReadReg( LIS2DH_STATUS_REG) & (0x8)) ) //wait until bit4 (XYZDA) data ready is high
    {
        asm("nop");
    }
    */
    uint8_t  mxl;
    uint8_t  mxh;
    uint8_t  myl;
    uint8_t  myh;
    uint8_t  mzl;
    uint8_t  mzh;

    // Read outputs
    mxl = i2cReadReg( (uint8_t)LIS2DH_OUT_X_L) ;
    mxh = i2cReadReg( (uint8_t)LIS2DH_OUT_X_H) ;

    myl = i2cReadReg( (uint8_t)LIS2DH_OUT_Y_L) ;
    myh = i2cReadReg( (uint8_t)LIS2DH_OUT_Y_H) ;

    mzl = i2cReadReg( (uint8_t)LIS2DH_OUT_Z_L) ;
    mzh = i2cReadReg( (uint8_t)LIS2DH_OUT_Z_H) ;

    // Combine high and low bytes and write into data structure
    accelerations[0] = (float)(  (int16_t)((mxh <<8) | mxl));
    accelerations[1] = (float)(  (int16_t)((myh <<8) | myl));
    accelerations[2] = (float)(  (int16_t)((mzh <<8) | mzl));
}

/******************************************************************************
 ******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************
 ******************************************************************************/