#pragma once
/*********************************************************************
 * @file   test_processing.h
 * @brief  Tests
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/



/*********************************************************************
   Definitions
*********************************************************************/

bool test_radonTransform( void );

bool test_detector_to_sinogram( void );

bool test_Tomography( void );

void serialisedToImage( void );

bool test_serialisation( void );

bool test_filter( void );