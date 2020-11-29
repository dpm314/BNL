/******************************************************************************
 *
 * @file   i2c_service.c
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
#include "i2c_service.h"
#include "lis2dh_manager.h"

//core
//drivers
//services
//managers
//libraries
#include <stdio.h>


/******************************************************************************
 ******************************************************************************
 * PRIVATE DATA
 ******************************************************************************
 ******************************************************************************/
#define LIS2DH_LOW_ADDRESS 0xC // 0b 00001100 (for SAD set to 0 V)

/******************************************************************************
 ******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************
 ******************************************************************************/


/******************************************************************************
 ******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************
 ******************************************************************************/


/******************************************************************************
 *
 * i2cWriteReg
 *
 ******************************************************************************/ 
void i2cWriteReg(uint8_t reg, uint8_t value)
{
    // Send the start command for a write command to the specified device
    I2CM1_MasterSendStart(LIS2DH_LOW_ADDRESS, I2CM1_WRITE_XFER_MODE);
    // Specify the register to write to.
    I2CM1_MasterWriteByte(reg);
    // Write the data byte
    I2CM1_MasterWriteByte(value);
    // Send the stop command
    I2CM1_MasterSendStop();
}

/******************************************************************************
 *
 * i2cReadReg
 *
 ******************************************************************************/ 
uint8_t i2cReadReg(uint8_t reg)
{
    uint8_t  rval ;
    
    // Send the start command for a read command to the specified device
    I2CM1_MasterSendStart(LIS2DH_LOW_ADDRESS, I2CM1_WRITE_XFER_MODE);
    // Specify the register to read from
    I2CM1_MasterWriteByte(reg);
    // Send a restart to read
    I2CM1_MasterSendRestart(LIS2DH_LOW_ADDRESS, I2CM1_READ_XFER_MODE);
    // Read byte from register
    rval = I2CM1_MasterReadByte(I2CM1_NAK_DATA);
    // Send the stop command
    I2CM1_MasterSendStop();
    
    return rval ;
}

/******************************************************************************
 ******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************
 ******************************************************************************/
