/*******************************************************************************
* WiTricity Corporation
* CONFIDENTIAL AND PROPRIETARY
* Copyright 2015 WiTricity Corporation
* All Rights Reserved
 
* NOTICE:  All information contained herein is and remains the property of 
* WiTricity Corporation and its licensors.  The intellectual and technical 
* concepts contained herein are proprietary to WiTricity and may be covered 
* by U.S. and foreign patents and patent applications, and are protected by 
* trade secret and copyright law.  Dissemination of this information or 
* reproduction of this material is strictly forbidden unless prior written 
* permission is obtained from WiTricity.
*******************************************************************************/


#ifndef EIG_H
    #define EIG_H

    // eig includes
    #include "knobs.h"  
    // Function prototypes
    #define PRINCIPLE_COMPONENTS 4
    #define MAT_SIZE 32
    
    void eig_decomp(float lambda[PRINCIPLE_COMPONENTS], float Phi[PRINCIPLE_COMPONENTS][MAT_SIZE], float Sigma[MAT_SIZE][MAT_SIZE], float Eig_vecs_init[PRINCIPLE_COMPONENTS][MAT_SIZE], const uint8 p_iter, const uint8 r_iter);
    float dot(float a[MAT_SIZE], float b[MAT_SIZE]);
    void invert_matrix(float Sigma[MAT_SIZE][MAT_SIZE], float SigmaInverse[MAT_SIZE][MAT_SIZE]);
    
#endif


/* [] END OF FILE */
