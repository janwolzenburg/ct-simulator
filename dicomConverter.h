#pragma once
/*********************************************************************
 * @file   dicomConverter.h
 *
 * @author Jan Wolzenburg
 * @date   April 2024
 *********************************************************************/


/*********************************************************************
	Includes
*********************************************************************/

#include <string>
using std::string, std::getline;

#include <vector>
using std::vector;

#include <filesystem>
using std::filesystem::path ,std::filesystem::exists;
using dirEntry = std::filesystem::directory_entry;
using dirIter = std::filesystem::directory_iterator;

#include <fstream>
using std::ifstream;

#include <algorithm>
using std::count; using std::find; using std::sort;

#include <limits>
using std::numeric_limits;

#include <iostream>
using std::cout, std::endl, std::cin;


#include "imebra/imebra.h"
#include "model.h"


/*********************************************************************
	Definitions
*********************************************************************/


const string MD_FILENAME("metadata.csv");				// Name of metadata file

const vector<string> metadataNames = {					// Names of metadata tagNames
	"Series UID",
	"Collection",
	"3rd Party Analysis",
	"Data Description URI",
	"Subject ID",
	"Study UID",
	"Study Description",
	"Study Date",
	"Series Description",
	"Manufacturer",
	"Modality",
	"SOP Class Name",
	"SOP Class UID",
	"Number of Images",
	"File Size",
	"File Location",
	"Download Timestamp"
};



// Get monochrome image from dicom dataset
imebra::MutableImage getMonoImage(imebra::DataSet dataSet){

	imebra::Image slice = dataSet.getImageApplyModalityTransform(0);								// Load image with rescale/intercept to houndsfield units

	// Read image parameters and apply transformations. Prepare data reader

	// Get slice parameters
	string colorSpace = slice.getColorSpace();

	// Image transformation chain
	imebra::TransformsChain tChain;

	// Colorspace transformation
	imebra::ColorTransformsFactory colorTransform;
	if (!colorTransform.isMonochrome(colorSpace)) {
		imebra::Transform toMonochrome = colorTransform.getTransform(colorSpace.c_str(), "MONOCHROME2");
		tChain.addTransform(toMonochrome);
	}

	// Transform image
	imebra::MutableImage mSlice = tChain.allocateOutputImage(slice, slice.getWidth(), slice.getHeight());
	tChain.runTransform(slice, 0, 0, slice.getWidth(), slice.getHeight(), mSlice, 0, 0);

	return mSlice;
}



/*
--------------------------------------------------------------------------------------------------------------------
Class for metadata of collection
*/

class metadata {

	public:

		// Construct with empty path
		metadata() :	absPath(""), 
						tagNames(metadataNames.size(), "null"),
						allSeries(0)
		{};

		// Construct with absolute path to directory and given filename
		metadata(const string _path, const string _fName) : absPath(_path + "\\" + _fName),
															tagNames(metadataNames.size(), "null"),
															allSeries(0)
		{
			acquire();
		};
	
		// Construct with absolute path to directory and standard filename
		metadata(const string _path) : metadata(_path, MD_FILENAME)
		{};
		
			
			
		// Set path to metadata file
		bool setPath(const string _path) {
			return setPath(_path, MD_FILENAME);
		};

		bool setPath(const string _path, const string _fName) {
			absPath = _path + "\\" + _fName;
			absPath = absPath.lexically_normal();
			return isValid();
		};


		// Acquire metadata from file at path
		bool acquire(const unsigned int minImages = 0) {
			// Check if path is valid 
			if (!isValid()) return false;;


			// Open metadata file

			ifstream mdFile;						// Metadata file
			mdFile.open(absPath, std::ios::in);		// Open file	

			// Check if file has been opened
			if (!mdFile.is_open()) return false;


			// Read metadata header

			string head;							// Head of metadata file
			getline(mdFile, head);					// Read first line

			// Iterate metadata tag names
			for (const auto& name : metadataNames) {
				// Position of tag name in header
				size_t namePos = head.find(name);

				// Name index as amount of commata before the name
				long long int nameIdx = count(head.begin(), head.begin() + namePos, ',');

				// Store tag name in vector at indexed position
				tagNames[nameIdx] = name;

			}

				
			// Read series in metadata file

			// Iterate all lines in metadata file
			for (string line; !getline(mdFile, line).eof();) {
					
				// Current series in this line
				vector<string> series;
				// Assign dafault null value
				series.assign(metadataNames.size(), "null");


				// Convert comma from file size to dot
				if (count(line.begin(), line.end(), ',') > (long long int) metadataNames.size() - 1) {
					size_t sizeUnitPos;

					sizeUnitPos = line.find(" KB");				// KB?
					if (sizeUnitPos == line.npos) {				// KB not found
						sizeUnitPos = line.find(" MB");			// MB?
						if (sizeUnitPos == line.npos) {			// MB not found
							sizeUnitPos = line.find(" GB");		// GB
							if (sizeUnitPos == line.npos) {		// GB not found
								std::cout << "Size unit not found!" << std::endl;
								return false;					// One comma too much in line
							}
						}
					}

					line[line.find_last_of(',', sizeUnitPos)] = '.';	// Replace comma with dot
				}

				bool storeSeries = true;		// Flag indicating to store series

				// Iterate all tag names
				for (vector<string>::size_type tagIdx = 0; tagIdx != series.size(); tagIdx++) {
		
					// Tag not available in metadata file
					if (tagNames[tagIdx] == "null") continue;
					
					size_t startPos = 0, endPos = 0;

					// Find start position of tag value in line
					for (size_t curComma = 0; curComma++ < tagIdx; startPos = line.find_first_of(',', startPos) + 1);
						
					// Find ending position of tag value in line
					endPos = line.find_first_of(',', startPos);

					// Get value
					string tagVal = line.substr(startPos, endPos - startPos);

					// Find index of current tag name in global context
					vector<string>::const_iterator tagNameIt = find(metadataNames.begin(), metadataNames.end(), tagNames[tagIdx]);
						
					if (tagNameIt == metadataNames.end()) continue;				// Not found -> continue to next
					size_t tagNameIdx = std::distance(metadataNames.begin(), tagNameIt);
						
					// Write to vector
					series.at(tagNameIdx) = tagVal;

					// Check modality and minimum images constraints
					if ((tagNames[tagIdx] == "Modality" && tagVal != "CT")	||
						(tagNames[tagIdx] == "Number of Images" && std::stoull(tagVal) < minImages))
							storeSeries = false;

				}

				// Add series to all series vector conditionally
				if (count(series.begin(), series.end(), "null") < (long long int) metadataNames.size() && storeSeries) 
					allSeries.push_back(series);

			}


			mdFile.close();							// Close file
			return true;			
		};


		// Valid path to metadata file
		inline bool isValid(void) const{ return exists(absPath); };
		
			
		// Amount of series in collection
		size_t numSeries(void) const {
			return allSeries.size();
		}

			
		// Get complete series tags in collection
		vector<string> getSeries(const size_t seriesIdx) const {
				
			// Series index in range
			if (seriesIdx >= allSeries.size()) return vector<string>(metadataNames.size(), "null");
			return allSeries[seriesIdx];
		};

			
		// Get path
		path getPath(void) const{ return absPath; };	



	// Private properties
	private:
		path absPath;								// Absolute path to metadata file
		vector<string> tagNames;					// Tag names included in header in ascending order
		vector<vector<string>> allSeries;			// Vector of series. One series is vector of strings with tag contents in order of metadataNames

};



/*
--------------------------------------------------------------------------------------------------------------------
Struct for image parameters
*/

struct imgPara{

	path fName = path("");			// Absolute path to dicom file
	unsigned int width = 0;			// Width of image
	unsigned int height = 0;		// Height of image
	double location = 0;			// Slice location in mm
	double thickness = 0;			// Slice thickness in mm
	double pxSpacRows = 1;			// Vertical pixel spacing
	double pxSpacCols = 1;			// Horizontal pixel spacing
};





/*
--------------------------------------------------------------------------------------------------------------------
Class for one series of collection
*/

class series {

	// Public functions
	public:
			
		// Constructor
		series(const class metadata _md, const size_t _seriesIdx) :
				md(_md),
				tags(md.getSeries(_seriesIdx)),		
				absPath(md.getPath().parent_path() / path(getTagValue("File Location")).lexically_normal()),
				images(vector<imgPara>()),
				model3D(Model{}),
				modelReady(false)
			
		{
			images.reserve(std::stoul(getTagValue("Number of Images")));
		};


		// Get tag value from tag name
		string getTagValue(const string tagName) const {
 
			// Index of tag 
			vector<string>::const_iterator tagIt;
			tagIt = find(metadataNames.begin(), metadataNames.end(), tagName);
			if (tagIt == metadataNames.end()) return "null";				// Return "null" when there is no match

			size_t tagIdx = std::distance(metadataNames.begin(), tagIt);	// Distance of iterators is tag index

			return tags.at(tagIdx);

		};

			
		// Process dicom files
		bool readDicomData(void) {
			acqImageList();
			if (images.size() < 1) return false;
				
			acqModelParameters();
				

			if (acqModelData() == false) return false;

			return true;
		};


		bool cropModel(const Tuple3D minCoords, const Tuple3D maxCoords) {
			return model3D.Crop( minCoords, maxCoords);
		}


		// Export model to file
		bool exportToFile(path file) { 
			vector<char> binary_data;
			model3D.Serialize( binary_data );
			return ExportSerialized( file, binary_data );
		}
			

	// Public properties
	public:
		metadata md;


	// Private properties
	private:
		vector<string> tags;						// Tag values in order of metadataNames
		path absPath;								// Path to dicom files
		vector<imgPara>	images;						// Vector of images
		Model model3D;								// 3D model of series
			
		bool modelReady;


	// Private functions
	private:

		// Get image list to process
		void acqImageList(void) {

			cout << endl << "Acquiring dicom image list..." << endl;
			cout << "\rFound " << std::setw(5) << 0 << " images";

			vector<path> allDcmFiles;

			// Iterate over all entries in directory
			for (const dirEntry& dirEntry : dirIter(absPath)) {

				if (dirEntry.is_regular_file()) {							// Entry is a file
					if (dirEntry.path().extension().string() == ".dcm") {
						allDcmFiles.push_back(dirEntry.path().filename());
					}
				}
			}


			// Get file names of dicom files and store in vector with slice locations in ascending order

			// Iterate over all dicom files found in directory
			for (vector<path>::iterator fIter = allDcmFiles.begin(); fIter < allDcmFiles.end(); fIter++) {

				path dcmFile = (absPath / *fIter);
				if (!exists(dcmFile)) continue;

				imebra::DataSet dataSet = imebra::CodecFactory::load(dcmFile, 0);		// Load DICOM dataset lazily

				// Skip file when it is not an axial image or does not have houndsfield unit as rescale output
				string imgType = dataSet.getString(imebra::TagId(imebra::tagId_t::ImageType_0008_0008), 2);
				string rescaleType = dataSet.getString(imebra::TagId(imebra::tagId_t::RescaleType_0028_1054), 0);
				string modality = dataSet.getString(imebra::TagId(imebra::tagId_t::Modality_0008_0060), 0);
				if (imgType != "AXIAL" || rescaleType != "HU" || rescaleType.length() == 0  || modality != "CT") continue;


				// Read image parameters
					
				imgPara tImgPara;
				tImgPara.fName = dcmFile;
				tImgPara.location = dataSet.getDouble(imebra::TagId(imebra::tagId_t::SliceLocation_0020_1041), 0);			// Read slice location
				tImgPara.thickness = dataSet.getDouble(imebra::TagId(imebra::tagId_t::SliceThickness_0018_0050), 0);	// Read slice thickeness
				tImgPara.width = dataSet.getUint32(imebra::TagId(imebra::tagId_t::Columns_0028_0011), 0);				// Read width
				tImgPara.height = dataSet.getUint32(imebra::TagId(imebra::tagId_t::Rows_0028_0010), 0);					// Read height
				tImgPara.pxSpacRows = dataSet.getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 0);		// Read pixel spacing
				tImgPara.pxSpacCols = dataSet.getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 1);		// Read pixel spacing


				// Store file name and slice location at correct location
				images.push_back(tImgPara);

				cout  << "\rFound " << std::setw(5) << images.size() << " images";

			}

			// Sort in ascending slice location
			std::sort(images.begin(), images.end(), [](imgPara img1, imgPara img2) { return img1.location < img2.location; });
			cout << endl;				

		};

				
		// Get size and spacial resolution of model
		void acqModelParameters(void) {

			// Resolution			

			// Image with minimum thickness
			vector<imgPara>::iterator minThicknessimgIt = std::min_element(images.begin(), images.end(), [](imgPara img1, imgPara img2) { return img1.thickness < img2.thickness; });
			double zRes = (*minThicknessimgIt).thickness;		// z-Resolution

			// Image with minimum vertical resolution
			vector<imgPara>::iterator minRowSpaceimgIt = std::min_element(images.begin(), images.end(), [](imgPara img1, imgPara img2) { return img1.pxSpacRows < img2.pxSpacRows; });
			double yRes = (*minThicknessimgIt).pxSpacRows;		// y-Resolution

			// Image with minimum horizontal resolution
			vector<imgPara>::iterator minColSpaceimgIt = std::min_element(images.begin(), images.end(), [](imgPara img1, imgPara img2) { return img1.pxSpacCols < img2.pxSpacCols; });
			double xRes = (*minThicknessimgIt).pxSpacCols;		// x-Resolution				

				
			// Size

			double zSize = images.back().location - images.front().location + zRes;		// Size of model in z-direction

			// Find maximum spacial size in x-direction
			vector<imgPara>::iterator maxWidthimgIt = std::max_element(images.begin(), images.end(), [](imgPara img1, imgPara img2) { return img1.pxSpacCols * img1.width < img2.pxSpacCols * img2.width; });
			double xSize = (*maxWidthimgIt).pxSpacCols * (*maxWidthimgIt).width;		// Size of model in x-direction

			// Find maximum spacial size in y-direction
			vector<imgPara>::iterator maxHeightimgIt = std::max_element(images.begin(), images.end(), [](imgPara img1, imgPara img2) { return img1.pxSpacRows * img1.height < img2.pxSpacRows* img2.height; });
			double ySize = (*maxHeightimgIt).pxSpacRows * (*maxHeightimgIt).height;		// Size of model in y-direction


			// Number of voxels in each direction				

			size_t xNum = (size_t) (xSize / xRes);
			size_t yNum = (size_t) (ySize / yRes);
			size_t zNum = (size_t) (zSize / zRes);
	

			// Save parameters to model member
			model3D = Model{ GetDummySystem(), Index3D(xNum, yNum, zNum), Tuple3D(xRes, yRes, zRes), "Model name" };

			cout  << endl << "Model data:" << endl;
			cout << "Voxel amount: " << xNum << ", " << yNum << ", " << zNum << endl;
			cout << "Voxel size: " << xRes << ", " << yRes << ", " << zRes << endl;
			cout << "Model size: " << xSize << ", " << ySize << ", " << zSize << endl;
				
	
		};
	
						
		// Get model data from images
		bool acqModelData(void) {	

			cout << endl << "\rProcessed " << std::setw(5) << 0 << "of " << model3D.number_of_voxel_3D().z << " model slices";

			// Iterate model voxels in z-direction
			for (size_t z = 0; z < model3D.number_of_voxel_3D().z; z++) {
					
				double sliceLoc = z * model3D.voxel_size().z + images.front().location;  // Slice location corresponding to z-Index
		
				// Find slice with minimal difference from its location to current z-location
				vector<imgPara>::iterator sliceIt = std::min_element(images.begin(), images.end(), [sliceLoc](const imgPara img1, const imgPara img2) { return abs(img1.location - sliceLoc) < abs(img2.location - sliceLoc); });

				if (!exists(sliceIt->fName)) return false;
				imebra::DataSet dataSet = imebra::CodecFactory::load(sliceIt->fName, 0);		// Load DICOM dataset lazily

				// Write pixel data to 3d-models voxels
				if (acqPixVoxData(dataSet, z) == false) return false;		

				cout << "\rProcessed " << std::setw(5) << z + 1 << " of " << model3D.number_of_voxel_3D().z << " model slices";

			}

			cout << endl << "Model data acquired!" << endl << endl;

			return true;
		};


		// Write pixel data from dicom image to voxels at z-Index of model
		bool acqPixVoxData(const imebra::DataSet dataSet, const size_t zIdx) {
				
			// Load monochrome image
			const imebra::MutableImage slice = getMonoImage(dataSet);								

			// Data handler for reading pixel values
			const imebra::ReadingDataHandlerNumeric dataHandler(slice.getReadingDataHandler());

			size_t dataLength;										// Number of bytes in image pixel data
			const char* data = dataHandler.data(&dataLength);		// Pointer to pixel data		
			size_t bytesPerValue = dataHandler.getUnitSize();		// Bytes per pixel

			// Check consistency  of image parameters
			if ((size_t)slice.getWidth() * slice.getHeight() * bytesPerValue != dataLength) return false;


			double pxSpaceRows = dataSet.getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 0);				// Vertical pixel spacing
			double pxSpaceCols = dataSet.getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 1);				// Horizontal pixel spacing

			// Iterate model voxels in y - direction
			for (size_t y = 0; y < model3D.number_of_voxel_3D().y; y++) {
				const double yPix = (double)y * model3D.voxel_size().y;					// y-Pixel location in mm
				const size_t yPixIdx = (size_t)(yPix / pxSpaceRows);				// y-Pixel index


				// Iterate model voxels in x - direction
				for (size_t x = 0; x < model3D.number_of_voxel_3D().x; x++) {
					const double xPix = (double)x * model3D.voxel_size().x;				// x-Pixel location in mm
					const size_t xPixIdx = (size_t)(xPix / pxSpaceCols);			// x-Pixel index						

					// Get raw data from image
					int hu = *((int*) (data + bytesPerValue * (yPixIdx * slice.getWidth() + xPixIdx)));
					if (hu < -1000) hu = -1000;			// Limit to resolve negative absoprtion coefficient

					//const double hu = slope * raw + intercept;							// Houndsfield units from raw data 
					VoxelData vox = { static_cast<double>(  hu * (absorption_water_Per_mm - absorption_air_Per_mm) / 1000 + absorption_water_Per_mm ), reference_energy_for_mu_eV };		// Absorption in voxel data

					// Write data to model
					model3D(Index3D{ x, y, zIdx }) = vox;

				}
			}

			return true;
		}

};
	