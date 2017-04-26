#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include <dirent.h>
#include <math.h>

#include "epipole.h"
#include "imgEdit.h"

#include "graphicSystem.h"
#include "inputSystem.h"

epipole::epipole(const char* path, const char* name)
{
	initIMG(path, name);
	runEpipole();
}

epipole::~epipole() 
{
	clearMemory();
}

/******************************************************************************************************/

//Initialize IMG
void epipole::initIMG(const char* path, const char* name)
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
	
	//user connect the procram with an output folder
	_imgOutputFolder = (char* )malloc(ARRAY_SIZE*sizeof(char));
	_imgOutputFolder[0] = '\0';
	strcpy(_imgOutputFolder, OUTPUT_DATA);
	
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
}

//System Core
void epipole::runEpipole()
{
	commandLine();
}

//Command Line
void epipole::commandLine()
{
	bool loop = true;
	while(loop) {
		char dec[100], buf[100];
		choices();
		myGetLine(dec);
		loop = checkChoice(dec);
		
		printf("Press ENTER to continue....");
		myGetLine(buf);
	}
}

//Choices
void epipole::choices()
{
	int i = 0;
	clearScreen();
	printf("%d)Exit program (type: exit)\n", i++);
	printf("\nTYPE: ");
}

//Clear Screen
void epipole::clearScreen()
{
	for(int i = 0; i <= 100; i++)
		printf("\n");
}

//Check Choice and Perform the actions
bool epipole::checkChoice(char* choice)
{
	//Exit the program
	if (strcmp(choice, "exit") == 0)
		exit(0);
	
	//Uknown Command
	else
		printf("Uknown Command!\n\n");
	
	return true;
}

/******************************************************************************************************/


/******************************************************************************************************/

void epipole::clearMemory()
{
	free(_imgPath);
	free(_ersFileName);
	free(_imgOutputFolder);
	
	free(_mainFilePath);
	
	free(_pixelTab);
}