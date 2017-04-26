#pragma once

class mergeBands
{
private:
	unsigned short int _imgWidth[3];
	unsigned short int _imgHeight[3];
	unsigned short int _imgBands[3];
	
	unsigned char* _pixelTab[3];
	
public:
	mergeBands(char* redBand, char* greenBand2, char* blueBand3, char* imgName);
};