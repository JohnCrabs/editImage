#pragma once

class epipole {
private:
	
	char* _imgPath;
	char* _ersFileName;
	char* _imgOutputFolder;
	char* _mainFilePath;
	char _data[20];
	unsigned char* _pixelTab;
	
	unsigned short int _imgWidth;
	unsigned short int _imgHeight;
	unsigned short int _imgBands;
	
	/******************************************************************************************************/
	
	void initIMG(const char* path, const char* name);
	void runEpipole();
	void commandLine();
	void choices();
	void clearScreen();
	bool checkChoice(char* choice);
	
	/******************************************************************************************************/
	
	
	
	/******************************************************************************************************/
	
	void clearMemory();
	
	/******************************************************************************************************/
	
	
public:
	epipole(const char* path, const char* name);
	~epipole();
};