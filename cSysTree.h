#pragma once
/*********************************************************************
 * @file   cSysTree.h
 * @brief  Class for collection of cartesian coordinate systems
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include "cartesian.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

 // Maximum amount of Coordinate systems in tree
constexpr size_t MAX_CSYS_IN_TREE = 64;

/*!
 * @brief Get singleton instance of cartesian system tree
 * @return Reference to tree
*/
cSysTree& CSYS_TREE( void );

/*!
 * @brief Get global coordinate system
 * @return Pointer to global system
*/
const cartCSys* GLOBAL_CSYS( void );

/*!
 * @brief Get dummy system
 * @return Pointer to dummy system
*/
cartCSys* DUMMY_CSYS( void );



/*!
 * @brief Class to store related coordinate systems. Singleton pattern
*/
class cSysTree : virtual public mathObj{

	public:

	/*!
	 * @brief Get single instance
	 * @return Reference to instance
	*/
	static cSysTree& getInstance();

	/*!
	 * @brief Convert coordinate tree's data
	 * @return String with tree's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Add system to tree
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param parent_ Pointer to parent system
	 * @param name_ Name of the system
	 * @return Pointer to new system
	*/
	cartCSys* addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const cartCSys* parent_, const string name_ );
	
	/*!
	 * @brief Constructor from serialized data
	 * @details Before calling this constructor check with static method validModelData( binbData, it ) whether the data is from model file
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	cartCSys* addCSys( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Add system to tree with global system as parent
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param name_ Name of the system
	 * @return Pointer to new system
	*/
	cartCSys* addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const string name_ );

	/*!
	 * @brief Add system to tree at origin of parent
	 * @param parent_ Parent system
	 * @param name_ Name
	 * @return Pointer to new system
	*/
	cartCSys* addCSys( const cartCSys* parent_, const string name_ );

	/*!
	 * @brief Ass system to tree at origin of global system
	 * @param name_ Name
	 * @return Pointer to new system
	*/
	cartCSys* addCSys( const string name_ );

	/*!
	 * @brief Get dummy system
	 * @return Pointer to dummy
	*/
	cartCSys* getDummy( void );

	/*!
	 * @brief Get global system
	 * @return Pointer to global system
	*/
	const cartCSys* getGlobal( void );

	/*!
	 * @brief Check if pointed to system is valid
	 * @param element System to check
	 * @return True when system is part of this tree
	*/
	bool validTreeElement( const cartCSys* const element ) const;


	private:

	/*!
	 * @brief Default Constructor
	 * @details Initializes all systems in array to default cartCSys
	*/
	cSysTree( void );

	/*!
	 * @brief Deleted copy constructor
	*/
	cSysTree( const cSysTree& tree_ ) = delete;

	/*!
	 * @brief Deleted assignment operator
	*/
	cSysTree& operator=( const cSysTree& tree_ ) = delete;


	private:
	size_t numSystems;			/*!< Number of systems in tree*/
	cartCSys* systems;			/*!< Array with all systems*/
};
