/*********************************************************
* @file   programState.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   March 2023
* ********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "programState.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


programState& PROGRAM_STATE( void ){
	return programState::getInstance();
}


programState& programState::getInstance(){
	static programState instance;
	return instance;
};


programState::programState( void ) :
	storedModelInstance{},
	storedModel{ getPath( "storedModel.model" ), storedModelInstance }
	//modelFileChooser{ "Choose CT model", "*.model", path{ "./" } },

	//currentModel{},
	//modelLoaded( false ),

	//modelSlice{},
	//modelSliceLoaded( false )
{

	// Check if state storage directory exists
	if( !std::filesystem::is_directory( stateStorage ) ) return;








	//// Where to find model file
	//const path modelFilePath = stateStorage / modelFilename;

	//// Check if model file exists
	//if( std::filesystem::exists( modelFilePath ) ){
	//	// Load model file
	//	//loadModel( modelFilePath );
	//}


	//const path modelChooserPath = stateStorage / modelChooserFilename;
	//if( std::filesystem::exists( modelChooserPath ) ){
	//	vector<char> binaryModelChooser = importSerialized( modelChooserPath.string() );
	//	vector<char>::iterator binaryModelChooserIt = binaryModelChooser.begin();
	//	modelFileChooser = fileChooser{ binaryModelChooser, binaryModelChooserIt };
	//}

	//const path modelSliceImagePath = stateStorage / modelSliceImageFilename;
	//if( std::filesystem::exists( modelSliceImagePath ) ){
	//	vector<char> binaryModelSliceImage = importSerialized( modelSliceImagePath.string() );
	//	vector<char>::iterator binaryModelSliceImageIt = binaryModelSliceImage.begin();
	//	modelSlice = greyImage{ binaryModelSliceImage, binaryModelSliceImageIt };
	//}

}
//
//
//void programState::saveState( void ) const{
//	
//	// Check if state storage directory erxists
//	if( !std::filesystem::is_directory( stateStorage ) ) std::filesystem::create_directory( stateStorage );
//
//	const path modelFilePath = stateStorage / modelFilename;
//
//	if( modelLoaded ){
//
//		vector<char> modelBinaryData;
//		currentModel.serialize( modelBinaryData );
//		exportSerialized( modelFilePath.string().c_str(), modelBinaryData );
//
//	}
//
//
//	const path modelChooserPath = stateStorage / modelChooserFilename;
//
//	vector<char> chooserBinaryData;
//	modelFileChooser.serialize( chooserBinaryData );
//	exportSerialized( modelChooserPath.string().c_str(), chooserBinaryData );
//
//}
//
//void programState::loadModel( const path modelFile ){
//
//	vector<char> binaryModel = importSerialized( modelFile.string() );
//	vector<char>::iterator binaryModelIt = binaryModel.begin();
//
//	if( !model::validModelData( binaryModel, binaryModelIt ) ){
//		cerr << "Invalid model file!\n";
//		return;
//	}
//
//	model choosenModel{ binaryModel, binaryModelIt };
//
//	currentModel = choosenModel;
//	modelLoaded = true;
//
//}
//
//
//void programState::loadModel( void ){
//
//	path choosenPath = modelFileChooser.choose();
//
//	if( choosenPath.empty() ) return;
//
//	loadModel( choosenPath );
//
//};
