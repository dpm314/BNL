/**************************************************************************//**
 *
 * @file   LIS2DH_manager.h
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
#ifndef _LIS2DH_MANAGER_H
#define _LIS2DH_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 ******************************************************************************
 * PUBLIC DATA
 ******************************************************************************
 ******************************************************************************/

#define LIS2DH_FASTEST_ODR               (uint8)0
#define LIS2DH_PROGRAM_OFFSET_REGISTERS  (uint8)1


#define  LIS2DH_STATUS_REG_AUX 0x07
#define  LIS2DH_TEMP_OUT_L     0x0C
#define  LIS2DH_TEMP_OUT_H     0x0D
#define  LIS2DH_INT_COUNTER_REG         0x0E
    
#define  LIS2DH_WHO_AM_I_REG   0x0F
#define  LIS2MDL_TEMP_CFG_REG 0x1F

#define  LIS2DH_CTRL_REG1      0x20
#define  LIS2DH_CTRL_REG2      0x21
#define  LIS2DH_CTRL_REG3      0x22
#define  LIS2DH_CTRL_REG4      0x23
#define  LIS2DH_CTRL_REG5      0x24
#define  LIS2DH_CTRL_REG6      0x25

#define  LIS2MDL_REFERENCE     0x26
#define  LIS2DH_STATUS_REG2    0x27

#define  LIS2DH_OUT_X_L        0x28
#define  LIS2DH_OUT_X_H        0x29
#define  LIS2DH_OUT_Y_L        0x2A
#define  LIS2DH_OUT_Y_H        0x2B
#define  LIS2DH_OUT_Z_L        0x2C
#define  LIS2DH_OUT_Z_H        0x2D

#define  LIS2DH_FIFO_CTRL_REG   0x2E
#define  LIS2DH_FIFO_SRC_REG    0x2F
#define  LIS2DH_INT1_CFG        0x30
#define  LIS2DH_INT1_SOURCE     0x31
#define  LIS2DH_INT1_THS        0x32
#define  LIS2DH_INT1_DURATION   0x33

    
/******************************************************************************
 ******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************
 ******************************************************************************/

void lis2dh_init();
void lis2dh_getAccelerationOutputs(float accelerations[3]) ;

#endif /*#ifndef _LIS2DH_MANAGER_H*/
 
