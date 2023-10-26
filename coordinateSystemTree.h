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
	 * @brief Convert coordinate tree's data
	 * @return String with tree's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Add system to tree
	 * @param origin_ Origin of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param parent_ Pointer to parent_ system
	 * @param name_ Name of the system
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const PrimitiveVector3 origin_, const PrimitiveVector3 ex_, const PrimitiveVector3 ey_, const PrimitiveVector3 ez_, const CoordinateSystem* parent_, const string name_ );
	
	/*!
	 * @brief Constructor from serialized data
	 * @details Before calling this constructor check with static method validModelData( binbData, it ) whether the data is from model file
	 * @param binary_data Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	CoordinateSystem* AddSystem( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Add system to tree with global system as parent_
	 * @param origin_ Origin of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param name_ Name of the system
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const PrimitiveVector3 origin_, const PrimitiveVector3 ex_, const PrimitiveVector3 ey_, const PrimitiveVector3 ez_, const string name_ );

	/*!
	 * @brief Add system to tree at origin_ of parent_
	 * @param parent_ Parent system
	 * @param name_ Name
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const CoordinateSystem* parent_, const string name_ );

	/*!
	 * @brief Ass system to tree at origin_ of global system
	 * @param name_ Name
	 * @return Pointer to new system
	*/
	CoordinateSystem* AddSystem( const string name_ );

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
	CoordinateSystemTree( const CoordinateSystemTree& tree_ ) = delete;

	/*!
	 * @brief Deleted assignment operator
	*/
	CoordinateSystemTree& operator=( const CoordinateSystemTree& tree_ ) = delete;


	private:
	size_t system_count_;									/*!< Number of systems_ in tree*/
	array<CoordinateSystem, max_systems_in_tree> systems_;	/*!< Array with all systems_*/
};
