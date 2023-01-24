#pragma once
/*********************************************************************
 * @file   tests_device.h
 * @brief  Tests for device classes
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

 #include "gantry.h"

bool test_tube(void);

bool test_detector( void );

bool test_gantry( void );

gantry getTestGantry( const idx2CR sinogramSize, const size_t raysInBeam );