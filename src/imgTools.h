#pragma once

enum UserDec_IT {EXIT, BANDS_IMG, NEGATIVE_IMG, CHANGE_LIGHT, CHANGE_CONTRAST, ROTATE_90_L, ROTATE_90_R, ROTATE_USER, 
					THRESHOLD_IMG, BIT2_IMG, SPATIAL_RES_PIXEL_SKIP, SPATIAL_RES_PIXEL_AVE, PRINT_HISTOGRAM,
					HISTOGRAM_EQUALIZATION, HISTOGRAM_NORMALIZATION, ERROR};

class imgTools
{
private:
	
	char* _imgPath;
	char* _ersFileName;
	char* _imgOutputFolder;
	char* _mainFilePath;
	
	unsigned short int _imgWidth;
	unsigned short int _imgHeight;
	unsigned short int _imgBands;
	
	unsigned char* _pixelTab;
	int _pixelVal[3][256];
	
	unsigned int _average;
	
	const int _histHeight = 1024;
	const int _histWidth  = 1024;
	
	char _data[20];
	
	/*************/
	/* FUNCTIONS */
	/*************/
	
	//Main Core Functions
	void initIMG(const char* path, const char* name);
	void clearStack();
	void runSystem();
	void mySystemPause();
	
	//Output Functions
	void printSizeIMG();
	void savePixelValuesIMG();
	
	//User Input Functions
	UserDec_IT returnUserInput();
	void printChoices();
	void clearScreen();
	
	//Image Transformation
	void findPixelMinMaxMedian(const int* tab, const int limit, int& min, int& max, int& median);
	void printStatsIMG();
	
	void negativeIMG();
	
	int getLightValue();
	void changeLightIMG();
	
	int getContrastValue();
	void changeContrastIMG();
	
	void rotateIMG_LEFT();
	void rotateIMG_RIGHT();
	
	void takeAngle(float& angle);
	void mySwap(int& num1, int& num2);
	void rotateIMG_USER_ANGLE();
	
	void divideBands();
	
	int getThresholdValue();
	int getThreshBands();
	int takeBand(const int bandNum, const int num);
	void thresholdIMG();
	
	void BIT_2_IMG();
	void spatialResolutionSkipOnePixel();
	void spatialResolutionAverageFourPixel();
	
	void findMinMax(const int* tab, const int limit, int& min, int& max);
	void printHistograms();
	void histogramEqualization();
	
	void valMinMax(const int* tab, const int limit, int& min, int& max);
	void histogramNormalization();
	
	public:
	
	//Constructor-Destructor
	imgTools(const char* path, const char* name);
	~imgTools();
};
