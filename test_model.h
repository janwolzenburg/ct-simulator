#pragma once
/*********************************************************************
 * @file   tests_model.h
 * @brief  Tests for 3D model
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/

#include "model.h"


model getTestModel( const cartCSys* const parent );

bool test_testModel( void );

bool test_modelTransmission( void );