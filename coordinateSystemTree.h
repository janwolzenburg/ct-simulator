#pragma once
/*********************************************************************
 * @file   CoordinateSystemTree.h
 * @brief  class for collection of cartesian coordinate systems_
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



/*********************************************************************
	Includes
*********************************************************************/
#include <array>
using std::array;
#include "coordinateSystem.h"



 /*********************************************************************
	Definitions
 *********************************************************************/



/*!
 * @brief get singleton instance of cartesian system tree
 * @return Reference to tree
*/
CoordinateSystemTree& CoordinateSystems( void );

/*!
 * @brief get global coordinate system
 * @return Pointer to global system
*/
const CoordinateSystem* GlobalSystem( void );

/*!
 * @brief get dummy system
 * @return Pointer to dummy system
*/
CoordinateSystem* DummySystem( void );



/*!
 * @brief class to store related coordinate systems_. Singleton pattern
*/
class CoordinateSystemTree : public MathematicalObject{

	 // maximum amount of Coordinate systems_ in tree
	static constexpr size_t max_systems_in_tree = 64;

	public:

	/*!
	 * @brief get single instance
	 * @return Reference to instance
	*/
	static CoordinateSystemTree& GetInstance();

	/*!
	 * @brief convert coordinate tree's data
	 * @return string with tree's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief get dummy system
	 * @return Pointer to dummy
	*/
	CoordinateSystem* GetDummy( void ){ return &( *systems_.begin() ); };

	/*!
	 * @brief get global system
	 * @return Pointer to global system
	*/
	const CoordinateSystem* GetGlobal( void ){ return &( systems_.at( 1 ) ); };

	/*!
	 * @brief add system to tree
	 * @param origin Origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @param parent_ Pointer to parent_ system
	 * @param name Name of the system
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const Primitivevector3 origin, const Primitivevector3 ex, const Primitivevector3 ey, const Primitivevector3 ez, const CoordinateSystem* parent, const string name );
	
	/*!
	 * @brief constructor from serialized data_
	 * @details Before calling this constructor check with static method validModelData( binbData, it ) whether the data is from model file
	 * @param binary_data reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	CoordinateSystem* AddSystem( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief add system to tree with global system as parent_
	 * @param origin Origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @param name Name of the system
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const Primitivevector3 origin, const Primitivevector3 ex, const Primitivevector3 ey, const Primitivevector3 ez, const string name );

	/*!
	 * @brief add system to tree at origin_ of parent_
	 * @param parent_ Parent system
	 * @param name Name
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const CoordinateSystem* parent, const string name );

	/*!
	 * @brief ass system to tree at origin_ of global system
	 * @param name Name
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const string name );

	/*!
	 * @brief check if pointed to system is valid
	 * @param element System to check
	 * @return true when system is part of this tree
	*/
	bool IsValidTreeElement( const CoordinateSystem* const element ) const;


	private:

	/*!
	 * @brief default Constructor
	 * @details Initialises all systems_ in array to default CoordinateSystem
	*/
	CoordinateSystemTree( void );

	/*!
	 * @brief deleted copy constructor
	*/
	CoordinateSystemTree( const CoordinateSystemTree& tree ) = delete;

	/*!
	 * @brief deleted assignment operator
	*/
	CoordinateSystemTree& operator=( const CoordinateSystemTree& tree ) = delete;


	private:
	size_t system_count_;									/*!< Number of systems_ in tree*/
	array<CoordinateSystem, max_systems_in_tree> systems_;	/*!< array with all systems_*/
};
