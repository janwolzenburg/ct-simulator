#pragma once
/*********************************************************************
 * @file   tests_model.h
 * @brief  Tests for 3D model
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/

#include "model.h"


Model getTestModel( const CoordinateSystem* const parent, const size_t res = 1 );

bool test_testModel( void );

bool test_modelTransmission( void );


void save_testModel( void );