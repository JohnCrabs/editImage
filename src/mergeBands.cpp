//Libraries from C
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>

//Other Libraries
#include <dirent.h>

#include "imgEdit.h"
#include "mergeBands.h"

mergeBands::mergeBands(char* redBand, char* greenBand, char* blueBand, char* imgName)
{
	char data[20] = "\0";
	strcat(data, DATA);
	
	char outputData[20] = "\0";
	strcat(outputData, OUTPUT_DATA);
	
	//Create ERS file for first band (RED_BAND)
	char band_RED_ers[] = "BandRed";
	GDAL_translateToERS(redBand, band_RED_ers);
	getImageSize_ERS(_imgHeight[0], _imgWidth[0], _imgBands[0], band_RED_ers);
	
	//Create ERS file for first band (GREEN_BAND)
	char band_GREEN_ers[] = "BandGreen";
	GDAL_translateToERS(greenBand, band_GREEN_ers);
	getImageSize_ERS(_imgHeight[1], _imgWidth[1], _imgBands[1], band_GREEN_ers);
	
	//Create ERS file for first band (BLUE_BAND)
	char band_BLUE_ers[] = "BandBlue";
	GDAL_translateToERS(blueBand, band_BLUE_ers);
	getImageSize_ERS(_imgHeight[2], _imgWidth[2], _imgBands[2], band_BLUE_ers);
	
	//Error checking
	for(int i = 0; i < 3; i++) {
		//printf("BAND_%d\nHEIGHT = %d\nWIDTH  = %d\nBANDS  = %d\n\n", i+1, _imgHeight[i], _imgWidth[i], _imgBands[i]);
		if(_imgHeight[0] != _imgHeight[i]) {
			printf("ERROR: WRONG BAND HEIGHT TO BAND %d\n\n", i+1);
			exit(0);
		} else if(_imgWidth[0] != _imgWidth[i]) {
			printf("ERROR: WRONG BAND WIDTH TO BAND %d\n\n", i+1);
			exit(0);
		} else if(_imgBands[i] != 1) {
			printf("ERROR: IMAGE BAND %d = %d! CAN'T MERGE\n\n", i+1, _imgBands[i]);
			exit(0);
		}
	}
	
	
	char mainFilePath[3][50] = {"output/data/BandRed", "output/data/BandGreen", "output/data/BandBlue"};
	//Alocate Memory and Keep Values of pixel
	for(int i = 0; i < 3; i++) {
		_pixelTab[i] = (unsigned char*)malloc((_imgWidth[0]*_imgHeight[0])*sizeof(char));
		getPixelValues_IMG(mainFilePath[i], _pixelTab[i], _imgHeight[0], _imgWidth[0], _imgBands[0]);
	}
	
	//Create the New Path
	char* pathNew;
	pathNew = (char*)malloc(100*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, data);
	strcat(pathNew, imgName);
	
	//Merge Bands
	short int pixelValue;
	FILE* outFile = fopen(pathNew, "w");
	for(int hght = 0; hght < _imgHeight[0]; hght++) {
		for(int bands = 0; bands < 3; bands++) {
			for(int wid = 0; wid < _imgWidth[0]; wid++) {
				pixelValue = (short int)_pixelTab[bands][(hght*_imgWidth[0]) + wid];
				fprintf(outFile, "%c", (char)pixelValue);
			}
		}
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(imgName, mainFilePath[0], data, "BandRed", _imgHeight[0], _imgWidth[0], 3);
	
	//Ask the user for the output format
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(200*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, outputData);
	strcat(newImageName, imgName);
	strcat(newImageName, "_merged");
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(newImageName);
	free(pathNew);
	for(int i = 0; i < 3; i++)
		free(_pixelTab[i]);
}