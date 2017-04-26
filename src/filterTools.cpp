//Libraries from C
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>

//Other Libraries
#include <dirent.h>

#include "imgEdit.h"
#include "filterTools.h"


int compar (const void * a, const void * b)
{
  if ( *(float*)a <  *(float*)b ) return -1;
  if ( *(float*)a >  *(float*)b ) return 1;
  
  return 0;
}

/******************************************************************************************************/

//Contsractor
filterTools::filterTools(const char* imgPath, const char* outName)
{
	initIMG(imgPath, outName);
	runSystem();
}

//Destructor
filterTools::~filterTools()
{
	clearStack();
}

/******************************************************************************************************/

//Initialize IMG
void filterTools::initIMG(const char* path, const char* name)
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
	_mainFilePath = (char*)malloc(ARRAY_SIZE*sizeof(char));
	_mainFilePath[0] = '\0';
	strcat(_mainFilePath, _data);
	strcat(_mainFilePath, _ersFileName);
	
	//Create ERS file and read the IMG Size
	GDAL_translateToERS(_imgPath, _ersFileName);
	getImageSize_ERS(_imgHeight, _imgWidth, _imgBands, _ersFileName);
	
	//Error Checking
	if(_imgBands != 1) {
		printf("ERROR: CAN'T FILTER AN IMAGE WITH %d BANDS!\n", _imgBands);
		exit(0);
	}
	
	//connect outputFolder
	_imgOutputFolder = (char* )malloc(ARRAY_SIZE*sizeof(char));
	_imgOutputFolder[0] = '\0';
	strcpy(_imgOutputFolder, OUTPUT_DATA);
	
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
}

//Creat the Stack
void filterTools::clearStack()
{
	free(_imgPath);
	free(_ersFileName);
	free(_imgOutputFolder);
	free(_mainFilePath);
	free(_pixelTab);
}

//The core of the program
void filterTools::runSystem()
{
	UserDec_FT userDecition;
	while(true) {
		switch(userDecition = returnUserInput()) {
			case UserDec_FT::EXIT: {
				exit(0);
			}
			case UserDec_FT::FILTER_USER: {
				filterUser();
				break;
			}
			case UserDec_FT::FILTER_MEDIAN: {
				filterMedian();
				break;
			}
			case UserDec_FT::FILTER_MIN: {
				filterMin();
				break;
			}
			case UserDec_FT::FILTER_MAX: {
				filterMax();
				break;
			}
			case UserDec_FT::FILTER_SOBEL: {
				filterSobel();
				break;
			}
			case UserDec_FT::ERROR: {
				printf("\nUnkown Command!");
				break;
			}
		}
		
		mySystemPause();
	}
}

//Pause the system till user press ENTER
void filterTools::mySystemPause()
{
	printf("\n\nPress any key...");
	char in;
	while((in = getchar()) != '\n')
		;
}

/******************************************************************************************************/

//Return user's input
UserDec_FT filterTools::returnUserInput()
{
	char choice[20];
	printChoices();
	myGetLine(choice);
	lowerCase(choice);
	
	if(strcmp(choice, "exit") == 0) {
		return UserDec_FT::EXIT;
	} else if(strcmp(choice, "fuser") == 0) {
		return UserDec_FT::FILTER_USER;
	} else if(strcmp(choice, "median") == 0) {
		return UserDec_FT::FILTER_MEDIAN;
	} else if(strcmp(choice, "min") == 0) {
		return UserDec_FT::FILTER_MIN;
	} else if(strcmp(choice, "max") == 0) {
		return UserDec_FT::FILTER_MAX;
	} else if(strcmp(choice, "sobel") == 0) {
		return UserDec_FT::FILTER_SOBEL;
	}
	
	return UserDec_FT::ERROR;
}

//User choices
void filterTools::printChoices()
{
	int i = 0;
	
	//clear screen
	clearScreen();
	
	//print choices
	printf("Choose what do you want to do:\n");
	printf("(%02d)Exit Program (type: exit)\n", i++);
	printf("(%02d)User Filter Input (type: fuser)\n", i++);
	printf("(%02d)Median Filter (type: median)\n", i++);
	printf("(%02d)Min Filter (type: min)\n", i++);
	printf("(%02d)Max Filter (type: max)\n", i++);
	printf("(%02d)Sobel Filter (type: sobel)\n", i++);
	
	printf("Type: ");
}

//Clear Screen
void filterTools::clearScreen()
{
	for(int i = 0; i <= 100; i++)
		printf("\n");
}

/******************************************************************************************************/

//User input the path of the filter
bool filterTools::getFilter(float& divider)
{
	char path[100];
	char filterBuffer[400][20];
	
	for(int i = 0; i < 400; i++)
		filterBuffer[i][0] = '\0';
	
	_filt_X_dim = 0;
	_filt_Y_dim = 0;
	
	//Take filter path from user
	printf("\nInsert the path of the filter file:\nType: ");
	myGetLine(path);
	
	//Read file and keep dimensions and filter values
	FILE* openFile = fopen(path, "r");
	if(!openFile) {
		printf("\nERROR: FILE %s NOT EXITS!", path);
		return true;
	}
	//read file
	int index = 0; 
	int count = 0;
	char c;
	while((c = fgetc(openFile)) != EOF) {
		//skip non numbers
		if(!isdigit(c) && c != '+' && c != '-' && c != '.')
			;
		//if is number
		else if(isdigit(c) || c == '+' || c == '-' || c == '.') {
			int i = 0;
			filterBuffer[index][i++] = c;
			while(isdigit(c = fgetc(openFile)) || c == '.')
				filterBuffer[index][i++] = c;
				
			//Error checking
			if (c == '\n' || c == EOF) {
					printf("\nERROR: WRONG FILTER FORMAT!\n");
					return true;
			}
			
			filterBuffer[index][i] = '\0';
			index++;
			
			count++;
			if(_filt_Y_dim == 0)
				_filt_X_dim++;
			
			//new line
			if(c == ';') {
				//Error checking
				if (count != _filt_X_dim) {
					printf("\nERROR: WRONG FILTER FORMAT!\n");
					return true;
				}
				_filt_Y_dim++;
				count = 0;
			}
		}
	}
	
	//Debugging
	/*printf("\n\nF_X = %d\nF_Y = %d\n\n", _filt_X_dim, _filt_Y_dim);
	for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++)
		printf("filt_%d = %s\n", i, filterBuffer[i]);*/
	
	//error checking
	if( (_filt_X_dim%2 == 0) ) {
		printf("\nERROR: WRONG FILTER FORMAT!\n");
		return true;
	}
	
	_filterTab = (float*)malloc((_filt_X_dim*_filt_Y_dim) * sizeof(float));
	
	divider = 0.0;
	for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++) {
		_filterTab[i] = atof(filterBuffer[i]);
		divider += _filterTab[i];
	}
	return false;
}

//User Input filter
void filterTools::filterUser()
{
	//initialize filter
	float divider;
	if(getFilter(divider))
		return ;
	if(divider == 0)
		divider = -1;
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_USER_FILTER");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Debugging
	/*for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++)
		printf("\nF_%d = %f", i, _filterTab[i]);*/
	
	int index_h = (_filt_Y_dim - 1) / 2 ;
	int index_w = (_filt_X_dim - 1) / 2 ;
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int wid = 0; wid < _imgWidth; wid++) {
			for(int img_h = (hght - index_h), filt_h = 0; img_h <= (hght + index_h); img_h++, filt_h++) {
				for(int img_w = (wid - index_w), filt_w = 0; img_w <= (wid + index_w); img_w++, filt_w++) {
					if((img_h >= 0 && img_h <= _imgHeight) && (img_w >= 0 && img_w <= _imgWidth))
						pixelValue += (int)_pixelTab[img_h*_imgWidth + img_w]*_filterTab[filt_h*_filt_X_dim+filt_w];
				}
			}
			pixelValue/=divider;
			if(divider == -1) {
				pixelValue += 128;
				if(pixelValue > 255)
					pixelValue = 255;
				else if(pixelValue < 0)
					pixelValue = 0;
			}
			else if(divider == 1) {
				pixelValue += _pixelTab[hght*_imgWidth + wid];
				pixelValue /= 2;
			}else {
				if(pixelValue > 255)
					pixelValue = 255;
				else if(pixelValue < 0)
					pixelValue = 0;
			}
			fprintf(outFile, "%c", (char)pixelValue);
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
	newImageName = (char*)malloc(ARRAY_SIZE*sizeof(char));
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
	
	//Deallocate Memory
	free(_filterTab);
	free(pathNew);
	free(newImageName);
}

//Median filter
void filterTools::filterMedian()
{
	//User input the size of filter
	printf("\nType the size of the filter (sizes: 3 - 5 - 7 - 8 - 11 - 13 - 15):\nType: ");
	char size[10];
	myGetLine(size);
	_filt_X_dim = atoi(size);
	//Error checking
	if(_filt_X_dim < 3 || _filt_X_dim > 15 || _filt_X_dim%2 == 0) {
		printf("ERROR: FILTER SIZE IS INCORRECT!");
		return ;
	}
	else
		_filt_Y_dim = _filt_X_dim;
	
	//Allocate Memory for _filterTab
	_filterTab = (float*)malloc((_filt_X_dim*_filt_Y_dim)*sizeof(float));
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_MEDIAN_FILTER");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Debugging
	/*for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++)
		printf("\nF_%d = %f", i, _filterTab[i]);*/
	
	int index_h = (_filt_Y_dim - 1) / 2 ;
	int index_w = (_filt_X_dim - 1) / 2 ;
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int wid = 0; wid < _imgWidth; wid++) {
			for(int img_h = (hght - index_h), filt_h = 0; img_h <= (hght + index_h); img_h++, filt_h++) {
				for(int img_w = (wid - index_w), filt_w = 0; img_w <= (wid + index_w); img_w++, filt_w++) {
					if((img_h >= 0 && img_h <= _imgHeight) && (img_w >= 0 && img_w <= _imgWidth))
						_filterTab[(filt_h*_filt_X_dim) + filt_w] = _pixelTab[img_h*_imgWidth + img_w];
				}
			}
			qsort(_filterTab, (_filt_X_dim*_filt_Y_dim), sizeof(float), compar);
			pixelValue = _filterTab[(_filt_X_dim*_filt_Y_dim - 1) / 2];
			
			fprintf(outFile, "%c", (char)pixelValue);
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
	newImageName = (char*)malloc(ARRAY_SIZE*sizeof(char));
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
	
	//Deallocate Memory
	free(_filterTab);
	free(pathNew);
	free(newImageName);
}

//Min filter
void filterTools::filterMin()
{
	//User input the size of filter
	printf("\nType the size of the filter (sizes: 3 - 5 - 7 - 8 - 11 - 13 - 15):\nType: ");
	char size[10];
	myGetLine(size);
	_filt_X_dim = atoi(size);
	//Error checking
	if(_filt_X_dim < 3 || _filt_X_dim > 15 || _filt_X_dim%2 == 0) {
		printf("ERROR: FILTER SIZE IS INCORRECT!");
		return ;
	}
	else
		_filt_Y_dim = _filt_X_dim;
	
	//Allocate Memory for _filterTab
	_filterTab = (float*)malloc((_filt_X_dim*_filt_Y_dim)*sizeof(float));
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_MIN_FILTER");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Debugging
	/*for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++)
		printf("\nF_%d = %f", i, _filterTab[i]);*/
	
	int index_h = (_filt_Y_dim - 1) / 2 ;
	int index_w = (_filt_X_dim - 1) / 2 ;
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int wid = 0; wid < _imgWidth; wid++) {
			for(int img_h = (hght - index_h), filt_h = 0; img_h <= (hght + index_h); img_h++, filt_h++) {
				for(int img_w = (wid - index_w), filt_w = 0; img_w <= (wid + index_w); img_w++, filt_w++) {
					if((img_h >= 0 && img_h <= _imgHeight) && (img_w >= 0 && img_w <= _imgWidth))
						_filterTab[(filt_h*_filt_X_dim) + filt_w] = _pixelTab[img_h*_imgWidth + img_w];
				}
			}
			qsort(_filterTab, (_filt_X_dim*_filt_Y_dim), sizeof(float), compar);
			pixelValue = _filterTab[0];
			
			fprintf(outFile, "%c", (char)pixelValue);
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
	newImageName = (char*)malloc(ARRAY_SIZE*sizeof(char));
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
	
	//Deallocate Memory
	free(_filterTab);
	free(pathNew);
	free(newImageName);
}

//Max filter
void filterTools::filterMax()
{
	//User input the size of filter
	printf("\nType the size of the filter (sizes: 3 - 5 - 7 - 8 - 11 - 13 - 15):\nType: ");
	char size[10];
	myGetLine(size);
	_filt_X_dim = atoi(size);
	//Error checking
	if(_filt_X_dim < 3 || _filt_X_dim > 15 || _filt_X_dim%2 == 0) {
		printf("ERROR: FILTER SIZE IS INCORRECT!");
		return ;
	}
	else
		_filt_Y_dim = _filt_X_dim;
	
	//Allocate Memory for _filterTab
	_filterTab = (float*)malloc((_filt_X_dim*_filt_Y_dim)*sizeof(float));
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_MAX_FILTER");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Debugging
	/*for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++)
		printf("\nF_%d = %f", i, _filterTab[i]);*/
	
	int index_h = (_filt_Y_dim - 1) / 2 ;
	int index_w = (_filt_X_dim - 1) / 2 ;
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int wid = 0; wid < _imgWidth; wid++) {
			for(int img_h = (hght - index_h), filt_h = 0; img_h <= (hght + index_h); img_h++, filt_h++) {
				for(int img_w = (wid - index_w), filt_w = 0; img_w <= (wid + index_w); img_w++, filt_w++) {
					if((img_h >= 0 && img_h <= _imgHeight) && (img_w >= 0 && img_w <= _imgWidth))
						_filterTab[(filt_h*_filt_X_dim) + filt_w] = _pixelTab[img_h*_imgWidth + img_w];
				}
			}
			qsort(_filterTab, (_filt_X_dim*_filt_Y_dim), sizeof(float), compar);
			pixelValue = _filterTab[(_filt_X_dim*_filt_Y_dim - 1)];
			
			fprintf(outFile, "%c", (char)pixelValue);
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
	newImageName = (char*)malloc(ARRAY_SIZE*sizeof(char));
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
	
	//Deallocate Memory
	free(_filterTab);
	free(pathNew);
	free(newImageName);
}

//Create a table image with sobel X
void filterTools::Sobel_axis_X(short int* Sobel_X, char* format, const char* name, const int add, const int div, const unsigned int limit)
{
	//Create Sobel filter X
	_filt_X_dim = 3;
	_filt_Y_dim = 3;
	
	_filterTab = (float*)malloc((_filt_X_dim*_filt_Y_dim)*sizeof(float));
	
	//1st row
	_filterTab[0*_filt_X_dim + 0] = -1.0;
	_filterTab[0*_filt_X_dim + 1] =  0.0;
	_filterTab[0*_filt_X_dim + 2] =  1.0;
	
	//2nd row
	_filterTab[1*_filt_X_dim + 0] = -2.0;
	_filterTab[1*_filt_X_dim + 1] =  0.0;
	_filterTab[1*_filt_X_dim + 2] =  2.0;
	
	//3rd row
	_filterTab[2*_filt_X_dim + 0] = -1.0;
	_filterTab[2*_filt_X_dim + 1] =  0.0;
	_filterTab[2*_filt_X_dim + 2] =  1.0;
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,name);
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Debugging
	/*for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++)
		printf("\nF_%d = %f", i, _filterTab[i]);*/
	
	int index_h = (_filt_Y_dim - 1) / 2 ;
	int index_w = (_filt_X_dim - 1) / 2 ;
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int wid = 0; wid < _imgWidth; wid++) {
			for(int img_h = (hght - index_h), filt_h = 0; img_h <= (hght + index_h); img_h++, filt_h++) {
				for(int img_w = (wid - index_w), filt_w = 0; img_w <= (wid + index_w); img_w++, filt_w++) {
					if((img_h >= 0 && img_h <= _imgHeight) && (img_w >= 0 && img_w <= _imgWidth))
						pixelValue += (int)_pixelTab[img_h*_imgWidth + img_w]*_filterTab[filt_h*_filt_X_dim+filt_w];
				}
			}
			
			pixelValue += add;
			pixelValue /= div;
			
			if( (pixelValue > add - (int)limit && pixelValue < add + (int)limit) || pixelValue > 255)
				pixelValue = 255;
			else if (pixelValue < 0)
				pixelValue = 0;
			
			Sobel_X[hght*_imgWidth+wid] = pixelValue;
			fprintf(outFile, "%c", (char)pixelValue);
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(ARRAY_SIZE*sizeof(char));
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
	
	//Deallocate Memory
	free(_filterTab);
	free(pathNew);
	free(newImageName);
	
}

//Create a table image with sobel X
void filterTools::Sobel_axis_Y(short int* Sobel_Y, char* format, const char* name, const int add, const int div, const unsigned int limit)
{
	//Create Sobel filter X
	_filt_X_dim = 3;
	_filt_Y_dim = 3;
	
	_filterTab = (float*)malloc((_filt_X_dim*_filt_Y_dim)*sizeof(float));
	
	//1st row
	_filterTab[0*_filt_X_dim + 0] = -1.0;
	_filterTab[0*_filt_X_dim + 1] = -2.0;
	_filterTab[0*_filt_X_dim + 2] = -1.0;
	
	//2nd row
	_filterTab[1*_filt_X_dim + 0] =  0.0;
	_filterTab[1*_filt_X_dim + 1] =  0.0;
	_filterTab[1*_filt_X_dim + 2] =  0.0;
	
	//3rd row
	_filterTab[2*_filt_X_dim + 0] =  1.0;
	_filterTab[2*_filt_X_dim + 1] =  2.0;
	_filterTab[2*_filt_X_dim + 2] =  1.0;
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName, name);
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Debugging
	/*for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++)
		printf("\nF_%d = %f", i, _filterTab[i]);*/
	
	int index_h = (_filt_Y_dim - 1) / 2 ;
	int index_w = (_filt_X_dim - 1) / 2 ;
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int wid = 0; wid < _imgWidth; wid++) {
			for(int img_h = (hght - index_h), filt_h = 0; img_h <= (hght + index_h); img_h++, filt_h++) {
				for(int img_w = (wid - index_w), filt_w = 0; img_w <= (wid + index_w); img_w++, filt_w++) {
					if((img_h >= 0 && img_h <= _imgHeight) && (img_w >= 0 && img_w <= _imgWidth))
						pixelValue += (int)_pixelTab[img_h*_imgWidth + img_w]*_filterTab[filt_h*_filt_X_dim+filt_w];
				}
			}
			
			pixelValue += add;
			pixelValue /= div;
			
			if( (pixelValue > add - (int)limit && pixelValue < add + (int)limit) || pixelValue > 255)
				pixelValue = 255;
			else if (pixelValue < 0)
				pixelValue = 0;
				
			Sobel_Y[hght*_imgWidth+wid] = pixelValue;
			fprintf(outFile, "%c", (char)pixelValue);
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(ARRAY_SIZE*sizeof(char));
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
	
	//Deallocate Memory
	free(_filterTab);
	free(pathNew);
	free(newImageName);
	
}

//Sobel Filter
void filterTools::filterSobel()
{
	//Allocate Memory for _Sobel_X
	_Sobel_X = (short int*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(short int));
	//Allocate Memory for _Sobel_Y
	_Sobel_Y = (short int*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(short int));
	
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	
	//Create Sobel Table X
	Sobel_axis_X(_Sobel_X, format, "_SOBEL_X", 0, 3, 0);
	//Create Sobel Table X
	Sobel_axis_Y(_Sobel_Y, format, "_SOBEL_Y", 0, 3, 0);
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_SOBEL_FILTER");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Debugging
	/*for(int i = 0; i < _filt_X_dim*_filt_Y_dim; i++)
		printf("\nF_%d = %f", i, _filterTab[i]);*/
	
	short int pV_X;
	short int pV_Y;
	
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight; hght++) {
		for(int wid = 0; wid < _imgWidth; wid++) {
			
			pV_X = _Sobel_X[hght*_imgWidth+wid];
			pV_Y = _Sobel_Y[hght*_imgWidth + wid];
			
			pixelValue = sqrt( (pV_X*pV_X)+(pV_Y*pV_Y) );
			
			if(pixelValue > 255)
				pixelValue = 255;
			else if(pixelValue < 0)
				pixelValue = 0;
			
			
			fprintf(outFile, "%c", (char)pixelValue);
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, _imgHeight, _imgWidth, _imgBands);
	
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(ARRAY_SIZE*sizeof(char));
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
	
	//Deallocate Memory
	free(_Sobel_X);
	free(_Sobel_Y);
	free(pathNew);
	free(newImageName);
}

