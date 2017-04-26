#pragma once

class imgCalc
{
private:
	
	int  _lines = 0;
	char _imgName[100][20];
	char _imgPath[100][100];
	
	char* _pixelTab[100];
	
	void initCalc(const char* path, const char* name);
	void printIMG();
	
	void imgADD(short int* table_1, short int* table_2);
	void imgSUB(short int* table_1, short int* table_2);
	void imgMUL(short int* table_1, short int* table_2);
	void imgDIV(short int* table_1, short int* table_2);
	
public:
	//Contructor-Destructor
	imgCalc(const char* path, const char* name);
	~imgCalc(){};
};