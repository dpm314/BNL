/**************************************************************************//**
 *
 * @file   eig.c
 * @author David DeBaun
 * @date   11/2/2015
 *
 * @brief This module performs eigen decomposition of a symmetric matrix into  
 * its principle components. 
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

#include "knobs.h"
#include "eig.h"
#include "math.h"


// external globals
extern float W1[MAT_SIZE][MAT_SIZE];                // workspace matrix 1
extern float W2[MAT_SIZE][MAT_SIZE];                // workspace matrix 2
extern float W3[MAT_SIZE][MAT_SIZE];                // workspace matrix 3

// static function prototypes
static void power_iteration(float Phi_row[MAT_SIZE], 
  float Sigma[MAT_SIZE][MAT_SIZE], float eig_vec[MAT_SIZE], const uint8 p_iter);
static float compute_rayleigh_quotient(float Sigma[MAT_SIZE][MAT_SIZE], 
  float eig_vec[MAT_SIZE]);
static float rayleigh_quotient_iteration(float Phi_row[MAT_SIZE], 
  float Sigma[MAT_SIZE][MAT_SIZE], float eig_vec[MAT_SIZE], const uint8 r_iter);
static void deflation(float Deflated_Sigma[MAT_SIZE][MAT_SIZE], 
  float Sigma[MAT_SIZE][MAT_SIZE], float eig_vec[MAT_SIZE], float lambda);

float dot(float a[MAT_SIZE], float b[MAT_SIZE]);

/**************************************************************************//**
 * eig_decomp
 *
 * @brief Primary public function that accepts a symmetric 2D array, 2D of 
 * initial eigenvector guesses, number of Power Iterations and Rayleigh 
 * Iterations, and updates an array of eigenvalues and a 2D array of eigenvectors
 * with computed eigenvalues and eigenvectors for the symmetric 2D array.
 *
 * @param[out] lambda Array that gets updated with PRINCIPLE_COMPONENTS number of 
 * the most dominant eigenvalues for Sigma.
 *
 * @param[out] Phi 2D array that gets updated with PRINCIPLE_COMPONENTS number of 
 * MAT_SIZE-length eigenvectors for Sigma.
 *
 * @param[in] Sigma 2D array representing the symmetric matrix to be decomposed.
 *
 * @param[in] Eig_vecs_init 2D array of the PRINCIPLE_COMPONENTS number of 
 * MAT_SIZE-length initial eigenvector guesses for iteration.
 *
 * @param[in] p_iter Number of Power Iterations for eigen decomp.
 *
 * @param[in] r_iter Number of Rayleigh Quotient iterations for eigen decomp.
 *
 * @return Nothing.
 *
 *****************************************************************************/

void eig_decomp(float lambda[PRINCIPLE_COMPONENTS], 
  float Phi[PRINCIPLE_COMPONENTS][MAT_SIZE], float Sigma[MAT_SIZE][MAT_SIZE], 
  float Eig_vecs_init[PRINCIPLE_COMPONENTS][MAT_SIZE], const uint8 p_iter, 
  const uint8 r_iter)
{
  uint8 i = 0;
    
  /* Conditionals are for flexibility in chosing any combo of Power or Rayleigh 
    Iterations - can be streamlined based on system testing */
  if (p_iter > 0)
  {
    // computing dominant eigenvector 
    power_iteration(Phi[0], Sigma, Eig_vecs_init[0], p_iter);
    if (0 == r_iter)
    {
      lambda[0] = compute_rayleigh_quotient(Sigma, Phi[0]);
    }
    else
    { /* r_iter > 0 */
      lambda[0] = rayleigh_quotient_iteration(Phi[0], Sigma, Phi[0], r_iter);
    }
  }
  else if (r_iter > 0) 
  {
    lambda[0] = rayleigh_quotient_iteration(Phi[0], Sigma, Eig_vecs_init[0], r_iter);
  } /* else, p_it == 0 and r_it == 0 and Phi won't be updated */
  
  for (i = 1; i < PRINCIPLE_COMPONENTS; ++i) // loop for each remaining PC
  {
    // perform deflation on matrix to remove the dominant eigenvector component
    if (1 == i) 
    {
      // updates W1 with deflated Sigma the first time
      deflation(W1, Sigma, Phi[0], lambda[0]); 
    } 
    else 
    {
      // updates W1 with deflated W1 after the first time
      deflation(W1, W1, Phi[i-1], lambda[i-1]);
    }

    if (p_iter > 0) 
    {
      // computing dominant eigenvector
      power_iteration(Phi[i], W1, Eig_vecs_init[i], p_iter); 
      if (0 == r_iter) 
      {
        lambda[i] = compute_rayleigh_quotient(W1, Phi[i]);
      } 
      else 
      { /* r_iter > 0 */                
        lambda[i] = rayleigh_quotient_iteration(Phi[i], W1, Phi[i], r_iter);
      }
    } 
    else if (r_iter > 0) 
    {
      lambda[i] = rayleigh_quotient_iteration(Phi[i], W1, Eig_vecs_init[i], r_iter);
    } /* else, p_it == 0 and r_it == 0 and Phi won't be updated */
  } // end loop for each remaining PC
    
  return;
}

/**************************************************************************//**
 * power_iteration
 *
 * @brief Static function for performing Power Iteration method on 2D symmetric
 * matrix to compute the dominant eigenvector from an initial eigenvector guess.
 * 
 * @param[out] Phi_row Array of length MAT_SIZE that gets updated with the 
 * computed dominant eigenvector for Sigma.
 * 
 * @param[in] Sigma 2D array representing the symmetric matrix to be decomposed.
 *
 * @param[in] eig_vec MAT_SIZE-length initial eigenvector guess for iteration.
 *
 * @param[in] p_iter Number of Power Iterations for eigen decomp.
 *
 * @return Nothing, void function.
 *
 *****************************************************************************/

static void power_iteration(float Phi_row[MAT_SIZE], 
  float Sigma[MAT_SIZE][MAT_SIZE], float eig_vec[MAT_SIZE], const uint8 p_iter)
{
  /* 
    Performs power iteration on Sigma using initial eigenvector guess v_eig and
    updates the row of Phi at pointer Phi_row
  */
  uint8 i = 0, j = 0;
  float norm_v = 0;
  float temp_vec[MAT_SIZE] = {0};
    
  for (i = 0; i < p_iter; ++i) 
  {
    if (0 == i)
    {
    /* temp_vec = Sigma * eig_vec, Matrix-vector multiplication */
      for (j = 0; j < MAT_SIZE; ++j) 
      {
        temp_vec[j] = dot(Sigma[j], eig_vec);
      }
    }
    else
    {
    /* temp_vec = Sigma * Phi_row, Matrix-vector multiplication */
      for (j = 0; j < MAT_SIZE; ++j) 
      {
        temp_vec[j] = dot(Sigma[j], Phi_row);
      }
    }
    
    norm_v = sqrt(dot(temp_vec, temp_vec));
        
    for (j = 0; j < MAT_SIZE; ++j) 
    {
    Phi_row[j] = temp_vec[j] / norm_v;
    }
  }    
  return;
}

/**************************************************************************//**
 * dot
 *
 * @brief Public function for computing dot product of two MAT_SIZE-length 
 * arrays.
 * 
 * @param[in] a One array of length MAT_SIZE.
 * 
 * @param[in] b One array of length MAT_SIZE.
 *
 * @return c Scalar dot product of the two arrays.
 *
 *****************************************************************************/

float dot(float a[MAT_SIZE], float b[MAT_SIZE])
{
  /* Returns the dot product of float arrays of length MAT_SIZE at pointers "a"
    and "b" in scalar float "c"  */
    
  uint8 i = 0;
  float c = 0;
        
  for (i = 0; i < MAT_SIZE; ++i) 
  {
    c += a[i] * b[i];
  }
    
  return c;
}

/**************************************************************************//**
 * compute_rayleigh_quotient
 *
 * @brief Static function for computing Rayleigh Quotient for symmetric matrix
 * and a vector. Used by Power Iteration and Rayleigh Iteration.
 *
 * @param[in] Sigma 2D array of dimension MAT_SIZE x MAT_SIZE.
 * 
 * @param[in] eig_vec One array of length MAT_SIZE.
 *
 * @return lambda_l Scalar value representing Rayleigh Quotient.
 *
 *****************************************************************************/

static float compute_rayleigh_quotient(float Sigma[MAT_SIZE][MAT_SIZE], 
  float eig_vec[MAT_SIZE])
{
  /* % calculate Rayleigh quotient, lambda_l
  lambda_l = eig_vec_T * Sigma * eig_vec, for Sigma symmetric */
    
  uint8 i = 0, j = 0;
  float lambda_l = 0, temp = 0;
        
  for (i = 0; i < MAT_SIZE; ++i) 
  {
    lambda_l += Sigma[i][i] * eig_vec[i] * eig_vec[i];
      for (j = i+1; j < MAT_SIZE; ++j) 
      {
        temp += Sigma[i][j] * eig_vec[i] * eig_vec[j];
      }
    lambda_l += 2 * temp;
    temp = 0;   
  }
    
  return lambda_l;
}

/**************************************************************************//**
 * rayleigh_quotient_iteration
 *
 * @brief Static function for performing Rayleigh Quotient Iteration method on 
 * 2D symmetric matrix to compute dominant eigenvector and eigenvalue using an
 * initial eigenvector guess.
 * 
 * @param[out] Phi_row Array of length MAT_SIZE that gets updated with the 
 * final computed dominant eigenvector result.
 * 
 * @param[in] Sigma 2D array representing the symmetric matrix to be decomposed.
 *
 * @param[in] eig_vec MAT_SIZE-length initial eigenvector guess for iteration.
 *
 * @param[in] r_iter Number of Rayleigh Quotient Iterations for eigen decomp.
 *
 * @return lambda_l Dominant computed eigenvalue.
 *
 *****************************************************************************/

static float rayleigh_quotient_iteration(float Phi_row[MAT_SIZE], 
  float Sigma[MAT_SIZE][MAT_SIZE], float eig_vec[MAT_SIZE], const uint8 r_iter)
{
  uint8 i = 0, j = 0, k = 0;
  float lambda_l = 0, norm_v = 0;
  float temp_vec[MAT_SIZE] = {0};
     
  /* compute Rayleigh's quotient initial time */
  lambda_l = compute_rayleigh_quotient(Sigma, eig_vec);
    
  for (i = 0; i < r_iter; ++i)
  {
    /* W2 = Sigma - lambda_l * I, where I is identity matrix */
    for (j = 0; j < MAT_SIZE; ++j) /* loop over rows */
    {  
      for (k = 0; k < MAT_SIZE; ++k) /* loop over columns */
      {
        if (j == k) /* subtract lambda_l from the diagonal elements */
        {
          W2[j][j] = Sigma[j][j] - lambda_l;
        }
        else
        {
          W2[j][k] = Sigma[j][k];
        }
      } /* end looping over columns */
    } /* end looping over rows */
        
    //Invert_Sigma(W2, W3, 0);
    /* looks like Invert_Sigma requires 2 separate
                 workspace matrices, and we don't want to modify Sigma or W1 */
    
    invert_matrix(W2, W3); // modified version of Invert_Sigma
    
    /*  temp_vec = Inv_Matrix * eig_vec, Matrix-vector multiplication */
    
    if (0 == i) // first time...
    {
      for (j = 0; j < MAT_SIZE; ++j)
      {
        temp_vec[j] = dot(W3[j], eig_vec); // ...start with eig_vec initial guess
      }
    }
    else // after first time...
    {
      for (j = 0; j < MAT_SIZE; ++j)
      {
        temp_vec[j] = dot(W3[j], Phi_row); //...feedback the previous result 
      }
    }
    
    /* compute norm for normalization */
    norm_v = sqrt(dot(temp_vec, temp_vec));
        
    /* normalize */
    for (j = 0; j < MAT_SIZE; ++j) 
    {
      Phi_row[j] = temp_vec[j] / norm_v;
    }
        
    /* compute Rayleigh's quotient */
    lambda_l = compute_rayleigh_quotient(Sigma, Phi_row);
    
  } /* end looping through Rayleigh Iterations */
    
  return lambda_l;
}

/**************************************************************************//**
 * deflation
 *
 * @brief Static function for performing Hotelling's Deflation on matrix Sigma.
 * Subtracts out the component from Sigma corresponding to the dominant 
 * eigenpair.
 *
 * @param[out] Deflated_Sigma 2D array of dimension MAT_SIZE x MAT_SIZE 
 * containing result of the deflation.
 * 
 * @param[in] Sigma 2D array of dimension MAT_SIZE x MAT_SIZE to be operated on
 * by deflation.
 *
 * @param[in] eig_vec Array representing the dominant eigenvector of Sigma.
 *
 * @param[in] lambda Scalar representing the dominant eigenvalue of Sigma.
 *
 * @return Nothing.
 *
 *****************************************************************************/

static void deflation(float Deflated_Sigma[MAT_SIZE][MAT_SIZE], 
  float Sigma[MAT_SIZE][MAT_SIZE], float eig_vec[MAT_SIZE], float lambda)
{
  uint8 i = 0, j = 0;
    
  for (i = 0; i < MAT_SIZE; ++i) 
  {
    for (j = 0; j < MAT_SIZE; ++j) 
    {
      Deflated_Sigma[i][j] = Sigma[i][j] - lambda * eig_vec[i] * eig_vec[j];
    }
  }  
}

/**************************************************************************//**
 * invert_matrix
 *
 * @brief Modified version of Invert_Sigma from "FODMath.c", computes inverse
 * of matrix.
 *
 * @param[out] SigmaInverse 2D array that contains the result of the matrix
 * inversion.
 *
 * @param[in] Sigma 2D array of dimension MAT_SIZE x MAT_SIZE to be inverted.
 *
 * @return Nothing.
 *
 *****************************************************************************/

void invert_matrix(float Sigma[MAT_SIZE][MAT_SIZE], float SigmaInverse[MAT_SIZE][MAT_SIZE])
{
    uint16 i,j,k;     
	float temp;
	
    // make Sigma Inverse the Identity matrix
	for (i = 0;i<MAT_SIZE;i++)
    {
        for (j=0;j<MAT_SIZE;j++)
        {
            if(i==j)
                SigmaInverse[i][j] = 1;
            else
                SigmaInverse[i][j] = 0;
        }
    }
		
	 for (i=0; i<MAT_SIZE; i++)
     {
        //Divide row by diagonal element of Sigma
        temp = Sigma[i][i];
        for (j=0; j<MAT_SIZE; j++)
        {
            SigmaInverse[i][j] /= temp; //  has a lot of 0s, 1st time
            Sigma[i][j] /= temp;
        }

		//Eliminate non-diagonal elements in
        for (k=0;k<MAT_SIZE;k++)
        {
            if(k!=i)
            {
            	temp = Sigma[k][i];
            	for (j=0;j<MAT_SIZE;j++)
            	{

                    SigmaInverse[k][j]-= temp*SigmaInverse[i][j];
                    Sigma[k][j]-= temp*Sigma[i][j];
            	}
        	}
    	}
        
	}
	// Make the inverse covariance matrix symmetric // ****may be able to take this out ...
	/*for (i = 0;i<MAT_SIZE;i++)
    {
        for (j=0;j<MAT_SIZE;j++)
        {
            SigmaInverse[i][j] = (SigmaInverse[i][j] + SigmaInverse[j][i])/2;
			SigmaInverse[j][i] = SigmaInverse[i][j];
        }
    }*/
}

/* [] END OF FILE */
