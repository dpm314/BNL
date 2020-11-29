/**************************************************************************//**
 *
 * @file   i2c_service.h
 * @author toby_lou & david meichle
 * @date   03-aug-2016 2:54:38 PM
 *
 * @brief
 *
 * @copyright
 * WiTricity Corporation
 * CONFIDENTIAL AND PROPRIETARY
 * Copyright 2015 WiTricity Corporation
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
#ifndef _I2C_SERVICE_H
#define _I2C_SERVICE_H


#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
 ******************************************************************************
 * PUBLIC DATA
 ******************************************************************************
 ******************************************************************************/


/******************************************************************************
 ******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************
 ******************************************************************************/

/**************************************************************************//**
 * 
 * @brief Write a byte value to the specified register of the specified device
 * 
 * @param device: Device I2C address
 * @param reg:    register address
 * @param value:  register write value
 *
 * @return void
 * 
 ******************************************************************************/
void i2cWriteReg( uint8_t reg, uint8_t value) ;


/**************************************************************************//**
 * 
 * @brief Read a byte value from the specified register of the specified device
 * 
 * @param device: Device I2C address
 * @param reg:    register address
 *
 * @return register value
 * 
 ******************************************************************************/
uint8_t i2cReadReg( uint8_t reg) ;


#endif /*#ifndef _I2C_SERVICE_H*/
 

