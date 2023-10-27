#pragma once
/*********************************************************************
 * @file   CoordinateSystemTree.h
 * @brief  Class for collection of cartesian coordinate systems_
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
 * @brief Get singleton instance of cartesian system tree
 * @return Reference to tree
*/
CoordinateSystemTree& CoordinateSystems( void );

/*!
 * @brief Get global coordinate system
 * @return Pointer to global system
*/
const CoordinateSystem* GlobalSystem( void );

/*!
 * @brief Get dummy system
 * @return Pointer to dummy system
*/
CoordinateSystem* DummySystem( void );



/*!
 * @brief Class to store related coordinate systems_. Singleton pattern
*/
class CoordinateSystemTree : public MathematicalObject{

	 // Maximum amount of Coordinate systems_ in tree
	static constexpr size_t max_systems_in_tree = 64;

	public:

	/*!
	 * @brief Get single instance
	 * @return Reference to instance
	*/
	static CoordinateSystemTree& GetInstance();

	/*!
	 * @brief Convert coordinate tree's data_
	 * @return String with tree's data_
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Get dummy system
	 * @return Pointer to dummy
	*/
	CoordinateSystem* GetDummy( void ){ return &( *systems_.begin() ); };

	/*!
	 * @brief Get global system
	 * @return Pointer to global system
	*/
	const CoordinateSystem* GetGlobal( void ){ return &( systems_.at( 1 ) ); };

	/*!
	 * @brief Add system to tree
	 * @param origin Origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @param parent_ Pointer to parent_ system
	 * @param name Name of the system
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez, const CoordinateSystem* parent, const string name );
	
	/*!
	 * @brief Constructor from serialized data_
	 * @details Before calling this constructor check with static method validModelData( binbData, it ) whether the data_ is from model file
	 * @param binary_data Reference to vector with binary data_
	 * @param it Iterator to start of data_ in vector
	*/
	CoordinateSystem* AddSystem( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Add system to tree with global system as parent_
	 * @param origin Origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @param name Name of the system
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez, const string name );

	/*!
	 * @brief Add system to tree at origin_ of parent_
	 * @param parent_ Parent system
	 * @param name Name
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const CoordinateSystem* parent, const string name );

	/*!
	 * @brief Ass system to tree at origin_ of global system
	 * @param name Name
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const string name );

	/*!
	 * @brief Check if pointed to system is valid
	 * @param element System to check
	 * @return True when system is part of this tree
	*/
	bool IsValidTreeElement( const CoordinateSystem* const element ) const;


	private:

	/*!
	 * @brief Default Constructor
	 * @details Initialises all systems_ in array to default CoordinateSystem
	*/
	CoordinateSystemTree( void );

	/*!
	 * @brief Deleted copy constructor
	*/
	CoordinateSystemTree( const CoordinateSystemTree& tree ) = delete;

	/*!
	 * @brief Deleted assignment operator
	*/
	CoordinateSystemTree& operator=( const CoordinateSystemTree& tree ) = delete;


	private:
	size_t system_count_;									/*!< Number of systems_ in tree*/
	array<CoordinateSystem, max_systems_in_tree> systems_;	/*!< Array with all systems_*/
};
