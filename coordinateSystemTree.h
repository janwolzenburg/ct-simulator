#pragma once
/*********************************************************************
 * @file   coordinateSystemTree.h
 * @brief  class for collection of cartesian coordinate systems
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
 * @return reference to tree
*/
CoordinateSystemTree& GetCoordinateSystemTree( void );

/*!
 * @brief get global coordinate system
 * @return pointer to global system
*/
const CoordinateSystem* GetGlobalSystem( void );

/*!
 * @brief get dummy system
 * @return pointer to dummy system
*/
CoordinateSystem* GetDummySystem( void );



/*!
 * @brief class to store related coordinate systems. singleton pattern
*/
class CoordinateSystemTree : public MathematicalObject{

	public:

	static constexpr size_t max_systems_in_tree = 64;	/*!< maximum amount of coordinate systems in tree */


	/*!
	 * @brief get single instance
	 * @return reference to instance
	*/
	static CoordinateSystemTree& GetInstance();

	/*!
	 * @brief convert coordinate tree's data
	 * @return string with tree's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief get dummy system
	 * @return pointer to dummy
	*/
	CoordinateSystem* GetDummy( void ){ return &( *systems_.begin() ); };

	/*!
	 * @brief get global system
	 * @return pointer to global system
	*/
	const CoordinateSystem* GetGlobal( void ){ return &( systems_.at( 1 ) ); };

	/*!
	 * @brief add system to tree
	 * @param origin origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @param parent pointer to parent system
	 * @param name name of the system
	 * @return pointer to new system
	*/
	CoordinateSystem* AddSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez, const CoordinateSystem* parent, const string name );
	
	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	CoordinateSystem* AddSystem( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief add system to tree with global system as parent
	 * @param origin origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @param name name of the system
	 * @return pointer to new system
	*/
	CoordinateSystem* AddSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez, const string name );

	/*!
	 * @brief add system to tree at origin of parent
	 * @param parent parent system
	 * @param name name
	 * @return pointer to new system
	*/
	CoordinateSystem* AddSystem( const CoordinateSystem* parent, const string name );

	/*!
	 * @brief add system to tree at origin of global system
	 * @param name name
	 * @return pointer to new system
	*/
	CoordinateSystem* AddSystem( const string name );

	/*!
	 * @brief check if pointed to system is valid
	 * @param element system to check
	 * @return true when system is part of this tree
	*/
	bool IsValidTreeElement( const CoordinateSystem* const element ) const;


	private:

	/*!
	 * @brief default constructor
	 * @details initialises all systems in array to default coordinate system
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
	size_t system_count_;																		/*!< number of systems in tree*/
	array<CoordinateSystem, max_systems_in_tree> systems_;	/*!< array with all systems*/
};
