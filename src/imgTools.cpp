//Libraries from C
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>

//Other Libraries
#include <dirent.h>

#include "imgEdit.h"
#include "imgTools.h"

const int ARRAY_SIZE_imgTOOLS = 100;
const int LONG_PATH_SIZE      = 200;

const double PI = 3.1415926535;

/******************************************************************************************************/

//Constructor
imgTools::imgTools(const char* path, const char* name)
{
	initIMG(path, name);
	runSystem();
}

//Destructor
imgTools::~imgTools()
{
	clearStack();
}

/******************************************************************************************************/

//Initialize IMG
void imgTools::initIMG(const char* path, const char* name)
{
	//Keep the path of the Image
	_imgPath = (char*)malloc((int)strlen(path)*sizeof(char));
	_imgPath[0] = '\0';
	strcpy(_imgPath, path);
	
	//Keep the name of ers file
	_ersFileName = (char*)malloc((int)strlen(name)*sizeof(char));
	_ersFileName[0] = '\0';
	strcat(_ersFileName, name);
	
	_data[0] = '\0';
	strcat(_data, DATA);
	
	//Keep the name of pixel file
	_mainFilePath = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	_mainFilePath[0] = '\0';
	strcat(_mainFilePath, _data);
	strcat(_mainFilePath, _ersFileName);
	
	//Create ERS file and read the IMG Size
	GDAL_translateToERS(_imgPath, _ersFileName);
	getImageSize_ERS(_imgHeight, _imgWidth, _imgBands, _ersFileName);
	
	//user connect the procram with an output folder
	_imgOutputFolder = (char* )malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	_imgOutputFolder[0] = '\0';
	strcpy(_imgOutputFolder, OUTPUT_DATA);
	
	savePixelValuesIMG();
	printStatsIMG();
}

//Crear the Stack
void imgTools::clearStack()
{
	free(_imgPath);
	free(_ersFileName);
	free(_imgOutputFolder);
	free(_mainFilePath);
}

//The core of the program
void imgTools::runSystem()
{
	UserDec_IT UserDecition;
	while(true) {
		switch(UserDecition = returnUserInput()) {
			case UserDec_IT::EXIT: {
				exit(0);
			}
			case UserDec_IT::BANDS_IMG:{
				divideBands();
				break;
			}
			case UserDec_IT::NEGATIVE_IMG:{
				negativeIMG();
				break;
			}
			case UserDec_IT::CHANGE_LIGHT:{
				changeLightIMG();
				break;
			}
			case UserDec_IT::CHANGE_CONTRAST: {
				changeContrastIMG();
				break;
			}
			case UserDec_IT::ROTATE_90_L: {
				rotateIMG_LEFT();
				break;
			}
			case UserDec_IT::ROTATE_90_R: {
				rotateIMG_RIGHT();
				break;
			}
			case UserDec_IT::ROTATE_USER: {
				rotateIMG_USER_ANGLE();
				break;
			}
			case UserDec_IT::THRESHOLD_IMG: {
				thresholdIMG();
				break;
			}
			case UserDec_IT::BIT2_IMG: {
				BIT_2_IMG();
				break;
			}
			case UserDec_IT::SPATIAL_RES_PIXEL_SKIP: {
				spatialResolutionSkipOnePixel();
				break;
			}
			case UserDec_IT::SPATIAL_RES_PIXEL_AVE: {
				spatialResolutionAverageFourPixel();
				break;
			}
			case UserDec_IT::PRINT_HISTOGRAM: {
				printHistograms();
				break;
			}
			case UserDec_IT::HISTOGRAM_EQUALIZATION: {
				histogramEqualization();
				break;
			}
			case UserDec_IT::HISTOGRAM_NORMALIZATION: {
				histogramNormalization();
				break;
			}
			case UserDec_IT::ERROR: {
				printf("\nUnkown Command!");
				break;
			}
		}
		
		mySystemPause();
	}
}

//Pause the system till user press ENTER
void imgTools::mySystemPause()
{
	printf("\n\nPress any key...");
	char in;
	while((in = getchar()) != '\n')
		;
}

/******************************************************************************************************/

//Prints the HEIGHT, WIDTH and BANDS to screen
void imgTools::printSizeIMG()
{
	printf("\n*******************");
	printf("\nIMAGE_HEIGHT = %d", _imgHeight);
	printf("\nIMAGE_WIDTH  = %d", _imgWidth);
	printf("\nIMAGE_BANDS  = %d", _imgBands);
	printf("\n*******************");
	printf("\n\n");
}

//Save pixelValues into a txt file
void imgTools::savePixelValuesIMG()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create and Allocate memory for OUTPUT_PATH
	char* savePath;
	savePath = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	savePath[0] = '\0';
	strcat(savePath, _mainFilePath);
	strcat(savePath, ".txt");
	
	//Export pixels to path
	FILE* file = fopen(savePath, "w");
	for (int hght = 0; hght < _imgHeight; hght++){
		for (int bands = 0; bands < _imgBands; bands++){
			for (int wid = 0; wid < _imgWidth; wid++)
				fprintf(file, "%03d  ", (int)_pixelTab [ ( (hght*_imgBands) + bands ) * _imgWidth + wid ]);
			fprintf(file, "\n\n");
		}
		fprintf(file, "\n\n\n\n");
	}
	fclose(file);
	
	//Deallocate Memory
	free(_pixelTab);
	free(savePath);
}

/******************************************************************************************************/

//Return user's input
UserDec_IT imgTools::returnUserInput()
{
	char choice[20];
	printChoices();
	myGetLine(choice);
	lowerCase(choice);
	
	if(strcmp(choice, "exit") == 0) {
		return UserDec_IT::EXIT;
	}else if (strcmp(choice, "bands") == 0) {
		return UserDec_IT::BANDS_IMG;
	} else if (strcmp(choice, "negative") == 0) {
		return UserDec_IT::NEGATIVE_IMG;
	} else if (strcmp(choice, "light") == 0) {
		return UserDec_IT::CHANGE_LIGHT;
	} else if (strcmp(choice, "contrast") == 0) {
		return UserDec_IT::CHANGE_CONTRAST;
	} else if (strcmp(choice, "rotatel") == 0) {
		return UserDec_IT::ROTATE_90_L;
	} else if (strcmp(choice, "rotater") == 0) {
		return UserDec_IT::ROTATE_90_R;
	} else if (strcmp(choice, "userot") == 0) {
		return UserDec_IT::ROTATE_USER;
	} else if (strcmp(choice, "thres") == 0) {
		return UserDec_IT::THRESHOLD_IMG;
	} else if (strcmp(choice, "img2bit") == 0) {
		return UserDec_IT::BIT2_IMG;
	} else if(strcmp(choice, "srskip") == 0) {
		return UserDec_IT::SPATIAL_RES_PIXEL_SKIP;
	} else if(strcmp(choice, "srave") == 0) {
		return UserDec_IT::SPATIAL_RES_PIXEL_AVE;
	} else if(strcmp(choice, "prthist") == 0) {
		return UserDec_IT::PRINT_HISTOGRAM;
	} else if(strcmp(choice, "eqhist") == 0) {
		return UserDec_IT::HISTOGRAM_EQUALIZATION;
	} else if(strcmp(choice, "normhist") == 0) {
		return UserDec_IT::HISTOGRAM_NORMALIZATION;
	}
	
	return UserDec_IT::ERROR;
}

//User choices
void imgTools::printChoices()
{
	int i = 0;
	
	//clear screen
	clearScreen();
	
	//print choices
	printf("Choose what do you want to do:\n");
	printf("(%02d)Exit Program (type: exit)\n", i++);
	//printf("(%02d)Image Stats Print (type: stats)\n", i++);
	printf("(%02d)Divide Image RGB to BANDS (type: bands)\n", i++);
	printf("(%02d)Change Image to Negative (type: negative)\n", i++);
	printf("(%02d)Change Image Light (type: light)\n", i++);
	printf("(%02d)Change Image Contrast (type: contrast)\n", i++);
	printf("(%02d)Rotate Image 90 degree LEFT (type: rotatel)\n", i++);
	printf("(%02d)Rotate Image 90 degree RIGHT (type: rotater)\n", i++);
	printf("(%02d)User Input Angle (degree) Image Rotation  (type: userot)\n", i++);
	printf("(%02d)Create 1-bit Image with threshold method (type: thres)\n", i++);
	printf("(%02d)Create 2-bit Image (type: img2bit)\n", i++);
	printf("(%02d)Change Spatial Resolution 1:2 by skipping pixel (type: srskip)\n", i++);
	printf("(%02d)Change Spatial Resolution 1:2 by average four pixel (type: srave)\n", i++);
	printf("(%02d)Print the histograms of image's bands (type: prthist)\n", i++);
	printf("(%02d)Perform histogram equalization (type: eqhist)\n", i++);
	printf("(%02d)Perform histogram normalization (type: normhist)\n", i++);
	
	printf("Type: ");
}

//Clear Screen
void imgTools::clearScreen()
{
	for(int i = 0; i <= 100; i++)
		printf("\n");
}

/******************************************************************************************************/

//Find Min Max Median
void imgTools::findPixelMinMaxMedian(const int* tab, const int limit, int& min, int& max, int& median)
{
	int sortedTab[256];
	int index = 0;
	
	for (int i = 0; i < limit; i++)
		if(tab[i] > 0)
			sortedTab[index++] = i;
	
	min = sortedTab[0];
	median = sortedTab[index/2];
	max = sortedTab[index-1];
}

//create negative Image
void imgTools::printStatsIMG()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[] = "_STASTISTICS";
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _imgOutputFolder);
	strcat(pathNew, _ersFileName);
	strcat(pathNew, outName);
	strcat(pathNew, ".txt");
	
	for(int i = 0; i < _imgBands; i++)
		for(int j = 0; j < 256; j++)
			_pixelVal[i][j] = 0;
	
	//Store the number of pixel Value per band
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				_pixelVal[bands][pixelValue]++;
			}
		}
	}
	
	unsigned int total;
	unsigned long long int standardDeviation;
	unsigned int* sumPixelValue;
	sumPixelValue = (unsigned int*)malloc(_imgBands*sizeof(int)); 
	
	for(int i = 0; i < _imgBands; i++) {
		total = 0;
		sumPixelValue[i] = 0;
		_average = 0;
		standardDeviation = 0;
		
		fprintf(outFile, "BAND %d\n\n", i+1);
		for(int j = 0; j < 256; j++) {
			fprintf(outFile, "%03d = %10d   ||    ", j, _pixelVal[i][j]);
			total += _pixelVal[i][j];
			sumPixelValue[i] += j*_pixelVal[i][j];
			if(j%3 == 0)
				fprintf(outFile, "\n");
		}
		_average = sumPixelValue[i]/total;
		for(int hght = 0; hght < _imgHeight; hght++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
						pixelValue = (int)_pixelTab[((hght*_imgBands) + i)*_imgWidth + wid];
						standardDeviation += pow(((int)_average-(int)pixelValue), 2);
					}
				}
				standardDeviation = sqrt(standardDeviation/(total-1));
				
				
				//Find min and max Value for every band
				int minVal;
				int maxVal;
				int medVal;
				findPixelMinMaxMedian(_pixelVal[i], 256, minVal, maxVal, medVal);
				
				fprintf(outFile, "\n\n");
				fprintf(outFile, "SumPixelValue      = %10u\n", sumPixelValue[i]);
				fprintf(outFile, "TOTAL              = %10u\n", total);
				fprintf(outFile, "MinimumPixelValue  = %10u\n", minVal);
				fprintf(outFile, "MaximumPixelValue  = %10u\n", maxVal);
				fprintf(outFile, "MedianPixelValue   = %10u\n", medVal);
				fprintf(outFile, "Average            = %10u\n", _average);
				fprintf(outFile, "StandardDeviation  = %10llu\n", standardDeviation);
				fprintf(outFile, "\n\n");
			}
	
	fclose(outFile);
	
	free(_pixelTab);
	free(sumPixelValue);
	free(pathNew);
}

//create negative Image
void imgTools::negativeIMG()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_NEGATIVE");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Create Negative
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				pixelValue = 255 - (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//ask the user for light input
int imgTools::getLightValue()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	int light;
	char num[10];
	printf("\n\nInput Light Value\nLightValue = ");
	myGetLine(num);
	light = atoi(num);
	
	if(light < -255)
		return -255;
	else if(light > 255)
		return 255;
	return light;
}

//create negative Image
void imgTools::changeLightIMG()
{
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_LIGHTCHANGED");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	int light = getLightValue();
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid] + light;
				if(pixelValue < 0)
					pixelValue = 0;
				else if(pixelValue > 255)
					pixelValue = 255;
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//ask the user for Constract input
int imgTools::getContrastValue()
{
	int constract;
	char num[10];
	printf("\n\nInput Constract Value (-15,15):");
	printf("\nIf you type something lower than -15 or bigger than 15\nvalue will automaticely change to -15 or 15 .");
	printf("\n\nConstractValue = ");
	myGetLine(num);
	constract = atoi(num);
	
	if(constract < -15)
		return -15;
	else if(constract > 15)
		return 15;
	return constract;
}

//create Image with different constract
void imgTools::changeContrastIMG()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_CONTRCHANGED");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	int contrast = getContrastValue();
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				pixelValue = (contrast * ( (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid] - _average) + _average);
				if(pixelValue < 0)
					pixelValue = 0;
				else if(pixelValue > 255)
					pixelValue = 255;
				fprintf(outFile, "%c", (unsigned char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//rotate Image to the Left
void imgTools::rotateIMG_LEFT()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_ROTATED_LEFT");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Rotate Left
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int wid = _imgWidth; wid > 0; wid--) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int hght = 0; hght < _imgHeight; hght++) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgWidth, _imgHeight, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//rotate Image to the Right
void imgTools::rotateIMG_RIGHT()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_ROTATED_RIGHT");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int wid = 0; wid < _imgWidth; wid++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int hght = _imgHeight; hght > 0; hght--) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgWidth, _imgHeight, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//User insert the angle
void imgTools::takeAngle(float& angle)
{
	char num[50];
	printf("\n\nInsert rotation angle:\nAngle = ");
	myGetLine(num);
	angle = atof(num);
}

//Swap two nums
void imgTools::mySwap(int& min, int& max)
{
	int buffer;
	if (min < max)
		return ;
	else{
		buffer = min;
		min = max;
		max = buffer;
	}
}

//rotate Image by User Input Angle 
void imgTools::rotateIMG_USER_ANGLE()
{
	int x1;
	int y1;
	
	int D1x = _imgWidth/2;
	int D1y = _imgHeight/2;
	
	float rotAngle;
	
	int D2x_min;
	int D2y_min;
	int D2x_max;
	int D2y_max;
	
	int add_x;
	int add_y;
	
	takeAngle(rotAngle);
	
	while(rotAngle < 0)
		rotAngle = (int)rotAngle%360 + 360.0;
	while(rotAngle > 360)
		rotAngle = (int)rotAngle%360;
	
	float mul_1 = cos(rotAngle*PI/180);
	float mul_2 = sin(rotAngle*PI/180);
	float mul_3 = (-sin(rotAngle*PI/180));
	float mul_4 = cos(rotAngle*PI/180);
	
	if ((rotAngle >= 0 && rotAngle <= 90) || (rotAngle > 180 && rotAngle <= 270)) {
		
		int angle_LU_x = -D1x; 
		int angle_LU_y = -D1y;
		
		int angle_LB_x = -D1x; 
		int angle_LB_y =  D1y;
		
		int angle_RU_x =  D1x; 
		int angle_RU_y = -D1y;
		
		int angle_RB_x = D1x; 
		int angle_RB_y = D1y;
		
		D2x_min = angle_LU_x*mul_1 + angle_LU_y*mul_2;
		D2y_min = angle_LB_x*mul_3 + angle_LB_y*mul_4;
		
		D2x_max = angle_RB_x*mul_1 + angle_RB_y*mul_2;
		D2y_max = angle_RU_x*mul_3 + angle_RU_y*mul_4;
		
		add_x = D1x;
		add_y = D1y;
	}
	else if ((rotAngle > 90 && rotAngle <= 180) || (rotAngle > 270 && rotAngle <= 360)) {
		int angle_LU_x = -D1x; 
		int angle_LU_y =  D1y;
		
		int angle_LB_x = -D1x; 
		int angle_LB_y = -D1y;
		
		int angle_RU_x =  D1x; 
		int angle_RU_y =  D1y;
		
		int angle_RB_x =  D1x; 
		int angle_RB_y = -D1y;
		
		D2x_min = angle_LU_x*mul_1 + angle_LU_y*mul_2;
		D2y_min = angle_LB_x*mul_3 + angle_LB_y*mul_4;
		
		D2x_max = angle_RB_x*mul_1 + angle_RB_y*mul_2;
		D2y_max = angle_RU_x*mul_3 + angle_RU_y*mul_4;
		
		add_x = D1x;
		add_y = D1y;
	}
	mySwap(D2x_min, D2x_max);
	mySwap(D2y_min, D2y_max);
	
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_ROTATED_USER_ANGLE");
	//Create New Output File
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int i = D2y_min; i < D2y_max; i++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int j = D2x_min; j < D2x_max; j++) {
				x1 = j * mul_1 + i * mul_3;
				y1 = j * mul_2 + i * mul_4;
				
				if ((x1+add_x < 0 || x1+add_x > _imgWidth) || (y1+add_y < 0 || y1+add_y > _imgHeight))
					pixelValue = 0;
				else
					pixelValue = _pixelTab[(((y1+add_y)*_imgBands)+bands)*_imgWidth + (x1+add_x)];
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, D2y_max-D2y_min, D2x_max-D2x_min, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//Divide image at MONO
void imgTools::divideBands()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[3][ARRAY_SIZE_imgTOOLS];
	//band1
	outName[0][0] = '\0';
	//strcat(outName[0],_ersFileName);
	strcat(outName[0],"_BAND1");
	//band2
	outName[1][0] = '\0';
	//strcat(outName[1],_ersFileName);
	strcat(outName[1],"_BAND2");
	//band3
	outName[2][0] = '\0';
	//strcat(outName[2],_ersFileName);
	strcat(outName[2],"_BAND3");
	
	char* pathNew;
	
	char format[10];
		//ask the user for the output image type
		printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
		myGetLine(format);
	
	for(int bands = 0; bands < _imgBands; bands++)
	{
		pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
		pathNew[0] = '\0';
		strcat(pathNew, _data);
		strcat(pathNew, outName[bands]);
		
		//Extract Bands
		short int pixelValue;
		FILE* outFile = fopen(pathNew, "w");
		for(int hght = 0; hght < _imgHeight; hght++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
		fclose(outFile);
		
		//create the new ERS file
		createNewFile_ERS(outName[bands], _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, 1);
		
		//create new name
		lowerCase(format);
		
		char* newImageName;
		newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
		newImageName[0] = '\0';
		strcat(newImageName, _imgOutputFolder);
		strcat(newImageName, _ersFileName);
		strcat(newImageName, outName[bands]);
		strcat(newImageName, ".");
		strcat(newImageName, format);
		
		//create the path of new ers
		strcat(pathNew, ".ers");
		
		//save image
		upperCase(format);
		GDAL_translateERStoIMG(pathNew, newImageName, format);
		
		free(newImageName);
	}
	
	free(_pixelTab);
	free(pathNew);
}

//ask the user for light input
int imgTools::getThresholdValue()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	int light;
	char num[10];
	printf("\n\nInput Threshold Value\nThresholdValue = ");
	myGetLine(num);
	light = atoi(num);
	
	if(light < -255)
		return -255;
	else if(light > 255)
		return 255;
	return light;
}

//return the number of Bands user wants to thresh
int imgTools::getThreshBands()
{
	char array[100];
	myGetLine(array);
	while(!isdigit(array[0]))
		myGetLine(array);
	int bands = atoi(array);
	return bands;
}

//return which bands user wants to thres
int imgTools::takeBand(const int bandNum, const int num)
{
	int band = 0;
	while(band < 1 || band > 3) {
		char array[100];
		array[0] = 'a';
		while(!isdigit(array[0])) {
			printf("\nBAND_%d = ", num);
			myGetLine(array);
		}
		band = array[0] - '0';
	}
	return (band - 1);
}

//Create a 1bit image using Threshold method
void imgTools::thresholdIMG()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_THRESHOLD_IMG");
	//Create New Path
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	int thresh[3];
	
	//Examine the bands in which threshold will be performed
	int bands[3] = {0, 1, 2};
	int bandsValue = _imgBands;
	if(bandsValue > 1) {
		printf("At how many bands do you want to threshold?\nBandsNum = ");
		int threshBands = getThreshBands();
		if(threshBands < _imgBands)
			for(int i = 0; i < threshBands; i++) {
				bands[i]  = takeBand(bandsValue, i+1);
			}
			bandsValue = threshBands;
	}
	for (int i = 0; i < bandsValue; i++) {
		printf("\nBand_%d", i+1);
		thresh[0] = getThresholdValue();
	}
	
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int k = 0; k < _imgBands; k++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + k)*_imgWidth + wid];
				for (int check = 0; check < bandsValue; check++)
					if(k == bands[check]) {
						if(pixelValue <= thresh[check])
							pixelValue = 0;
						else if(pixelValue > thresh[check])
							pixelValue = 255;
					}
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//Create a 2bit
void imgTools::BIT_2_IMG()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_2BIT_IMG");
	//Create New Path
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Create 2-bit image
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				if(pixelValue < 64)
					pixelValue = 0;
				else if(pixelValue >= 64 && pixelValue < 128)
					pixelValue = 85;
				else if(pixelValue >= 128 && pixelValue < 187)
					pixelValue = 170;
				else if(pixelValue >= 187)
					pixelValue = 255;
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//Spatial resolution 1/2 by skipping 1-pixel
void imgTools::spatialResolutionSkipOnePixel()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_SR_SKIP");
	//Create New Path
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Change resolution
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 1; hght < _imgHeight; hght+=2) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 1; wid < _imgWidth; wid+=2) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight/2, _imgWidth/2, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//Spatial resolution 1/2 by average 4-pixel
void imgTools::spatialResolutionAverageFourPixel()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_SR_AVE");
	//Create New Path
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Change resolusion
	short int pixelValue;
	int index;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 1; hght < _imgHeight; hght+=2) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 1; wid < _imgWidth; wid+=2) {
				index = 0;
				for(int i = hght; i >= hght-1; i--)
					for(int j = wid; j >= wid-1; j--) {
						pixelValue += (short int)_pixelTab[((i*_imgBands) + bands)*_imgWidth + j];
						index++;
					}
				pixelValue/=(index+1);
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight/2, _imgWidth/2, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//Find the min and max value from values into a table
void imgTools::findMinMax(const int* tab, const int limit, int& min, int& max)
{
	min = tab[0];
	max = tab[0];
	for(int i = 0; i < limit; i++) {
		//find min
		if(min > tab[i])
			min = tab[i];
		
		//find max
		if(max < tab[i])
			max = tab[i];
	}
}

//Print a histogram for every Band of the Image 
void imgTools::printHistograms()
{
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	
	char col[3][5] = {"_1", "_2", "_3"};
	
	for(int bands = 0; bands < _imgBands; bands++) {
		
		int minVal;
		int maxVal;
		
		//find the min and max value of histogram
		findMinMax(_pixelVal[bands], 256, minVal, maxVal);
		
		float freq;
		
		if(minVal != 0)
			freq = maxVal/minVal;
		else
			freq = maxVal;
		//If freq is too big change it to a normal value
		if (freq > 100.0)
			freq/=10.0;
		
		float index = (_histHeight - 30) / (maxVal / freq);
		
		//printf("\n\nminVal = %d\nmaxVal = %d\nfreq = %f", minVal, maxVal, freq);
		
		//Create the histogram export values
		unsigned int histVal[256];
		for(int i = 0; i < 256; i++)
			histVal[i] = (int)((_pixelVal[bands][i] / freq) * index);
		
		//for Debugging
		/*char pathN[20];
		strcat(pathN, "histDebug");
		strcat(pathN, col[bands]);
		FILE* outFile = fopen(pathN, "w");
		for(int i = 0; i < 256; i++)
			fprintf(outFile, "%d = %d\n", i, histVal[i]);
		fclose(outFile);*/
		
		//Create New Output File
		char  outName[ARRAY_SIZE_imgTOOLS];
		outName[0] = '\0';
		strcat(outName,_ersFileName);
		strcat(outName,"_histBand");
		strcat(outName,col[bands]);
		//Create New Path
		char* pathNew;
		pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
		pathNew[0] = '\0';
		strcat(pathNew, _data);
		strcat(pathNew, outName);
		
		unsigned char* histogram;
		histogram = (unsigned char*)malloc((_histHeight*_histWidth*_imgBands)*sizeof(char));
		
		short int pixelValue = 0;
		for(int hght = 0; hght < _histHeight; hght++) 
			for(int chan = 0; chan < _imgBands; chan++)
				for(int wid = 0; wid < _histWidth; wid++)
					histogram[(hght*_imgBands + chan)*_histWidth + wid] = (char)pixelValue;
				
		//create hist table
		int indexH = _histHeight;
		int i;
		for(int hght = 0; hght < _histHeight; hght++) {
			i=0;
			for(int wid = 0; wid < _histWidth; wid+=(_histWidth/256)) {
				if ((int)histVal[i] >= indexH) {
					pixelValue = 255;
				} else {
					pixelValue = 0;
				}
				for(int j = 0; j < (_histWidth/255); j++)
					histogram[(hght*_imgBands + bands)*_histWidth + wid] = (char)pixelValue;
				i++;
			}
			--indexH;
		}
		
		//printf("\n** %d **\n", i);
		
		//create hist file
		FILE* outFile = fopen(pathNew, "w");
		for(int hght = 0; hght < _histHeight; hght++)
			for(int chan = 0; chan < _imgBands; chan++)
				for(int wid = 0; wid < _histWidth; wid++) 
					fprintf(outFile, "%c", histogram[(hght*_imgBands + chan)*_histWidth + wid]);
		fclose(outFile);
		
		//create the new ERS file
		createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _histHeight, _histWidth, _imgBands);
		
		//create new name
		lowerCase(format);
		
		char* newImageName;
		newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
		newImageName[0] = '\0';
		strcat(newImageName, _imgOutputFolder);
		//strcat(newImageName, _ersFileName);
		strcat(newImageName, outName);
		strcat(newImageName, ".");
		strcat(newImageName, format);
		
		//create the path of new ers
		strcat(pathNew, ".ers");
		
		//save image
		upperCase(format);
		GDAL_translateERStoIMG(pathNew, newImageName, format);
		
		free(pathNew);
		free(newImageName);
		free(histogram);
	}
	
}

//Export an image with histogram Equalization
void imgTools::histogramEqualization()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_HIST_EQ");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Histogram Equalization
	float freq;
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				freq = 0;
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				for(int i = 0; i <= pixelValue; i++)
					freq += _pixelVal[bands][i];
				freq = (freq/(_imgHeight*_imgWidth))*255;
				pixelValue = (int)freq;
				
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}

//Find the min and max value
void imgTools::valMinMax(const int* tab, const int limit, int& min, int& max)
{
	//find max
	for (int i = limit-1; i >= 0; i--)
		if(tab[i] != 0) {
			max = i;
			break;
		}
	//find min
	for (int i = 0; i < limit; i++)
		if(tab[i] != 0) {
			min = i;
			break;
		}
}

//Normalize histogram
void imgTools::histogramNormalization()
{
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//Create New Output File
	char  outName[ARRAY_SIZE_imgTOOLS];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_HIST_NORM");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE_imgTOOLS*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//find min/max for every band
	int minVal[3];
	int maxVal[3];
	
	for(int i = 0; i < _imgBands; i++)
		valMinMax(_pixelVal[i], 256, minVal[i], maxVal[i]);
	
	//g_min, g_max
	float g_min = 0.0;
	float g_max = 255.0;
	
	
	
	
	//Histogram Equalization
	float freq;
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(int wid = 0; wid < _imgWidth; wid++) {
				pixelValue = (short int)_pixelTab[((hght*_imgBands) + bands)*_imgWidth + wid];
				freq = (g_max - g_min) / (maxVal[bands] - minVal[bands]);
				freq = freq * (float)(pixelValue - minVal[bands]);
				pixelValue = freq + g_min;
				
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(LONG_PATH_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(_pixelTab);
	free(pathNew);
	free(newImageName);
}


