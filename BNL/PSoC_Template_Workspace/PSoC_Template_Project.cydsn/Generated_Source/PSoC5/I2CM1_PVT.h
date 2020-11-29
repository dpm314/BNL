/*******************************************************************************
* File Name: .h
* Version 3.50
*
* Description:
*  This file provides private constants and parameter values for the I2C
*  component.
*
* Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_PVT_I2CM1_H)
#define CY_I2C_PVT_I2CM1_H

#include "I2CM1.h"

#define I2CM1_TIMEOUT_ENABLED_INC    (0u)
#if (0u != I2CM1_TIMEOUT_ENABLED_INC)
    #include "I2CM1_TMOUT.h"
#endif /* (0u != I2CM1_TIMEOUT_ENABLED_INC) */


/**********************************
*   Variables with external linkage
**********************************/

extern I2CM1_BACKUP_STRUCT I2CM1_backup;

extern volatile uint8 I2CM1_state;   /* Current state of I2C FSM */

/* Master variables */
#if (I2CM1_MODE_MASTER_ENABLED)
    extern volatile uint8 I2CM1_mstrStatus;   /* Master Status byte  */
    extern volatile uint8 I2CM1_mstrControl;  /* Master Control byte */

    /* Transmit buffer variables */
    extern volatile uint8 * I2CM1_mstrRdBufPtr;   /* Pointer to Master Read buffer */
    extern volatile uint8   I2CM1_mstrRdBufSize;  /* Master Read buffer size       */
    extern volatile uint8   I2CM1_mstrRdBufIndex; /* Master Read buffer Index      */

    /* Receive buffer variables */
    extern volatile uint8 * I2CM1_mstrWrBufPtr;   /* Pointer to Master Write buffer */
    extern volatile uint8   I2CM1_mstrWrBufSize;  /* Master Write buffer size       */
    extern volatile uint8   I2CM1_mstrWrBufIndex; /* Master Write buffer Index      */

#endif /* (I2CM1_MODE_MASTER_ENABLED) */

/* Slave variables */
#if (I2CM1_MODE_SLAVE_ENABLED)
    extern volatile uint8 I2CM1_slStatus;         /* Slave Status  */

    /* Transmit buffer variables */
    extern volatile uint8 * I2CM1_slRdBufPtr;     /* Pointer to Transmit buffer  */
    extern volatile uint8   I2CM1_slRdBufSize;    /* Slave Transmit buffer size  */
    extern volatile uint8   I2CM1_slRdBufIndex;   /* Slave Transmit buffer Index */

    /* Receive buffer variables */
    extern volatile uint8 * I2CM1_slWrBufPtr;     /* Pointer to Receive buffer  */
    extern volatile uint8   I2CM1_slWrBufSize;    /* Slave Receive buffer size  */
    extern volatile uint8   I2CM1_slWrBufIndex;   /* Slave Receive buffer Index */

    #if (I2CM1_SW_ADRR_DECODE)
        extern volatile uint8 I2CM1_slAddress;     /* Software address variable */
    #endif   /* (I2CM1_SW_ADRR_DECODE) */

#endif /* (I2CM1_MODE_SLAVE_ENABLED) */

#if ((I2CM1_FF_IMPLEMENTED) && (I2CM1_WAKEUP_ENABLED))
    extern volatile uint8 I2CM1_wakeupSource;
#endif /* ((I2CM1_FF_IMPLEMENTED) && (I2CM1_WAKEUP_ENABLED)) */


#endif /* CY_I2C_PVT_I2CM1_H */


/* [] END OF FILE */
