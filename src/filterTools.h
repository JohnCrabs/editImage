#pragma once

enum class UserDec_FT {EXIT, FILTER_USER, FILTER_MEDIAN, FILTER_MIN, FILTER_MAX, FILTER_SOBEL, ERROR};

class filterTools
{
private:
	int _filt_X_dim;
	int _filt_Y_dim;
	
	char* _imgPath;
	char* _ersFileName;
	char* _imgOutputFolder;
	char* _mainFilePath;
	
	unsigned short int _imgWidth;
	unsigned short int _imgHeight;
	unsigned short int _imgBands;
	
	unsigned char* _pixelTab;
	
	float* _filterTab;
	short int* _Sobel_X;
	short int* _Sobel_Y;
	
	char _data[20];
	
	/*************/
	/* FUNCTIONS */
	/*************/
	
	//Main Core Functions
	void initIMG(const char* path, const char* name);
	void clearStack();
	void runSystem();
	void mySystemPause();
	
	//User Input Functions
	UserDec_FT returnUserInput();
	void printChoices();
	void clearScreen();
	
	//Filter Functions
	bool getFilter(float& divider);
	void filterUser();
	
	void filterMedian();
	void filterMin();
	void filterMax();
	
	void Sobel_axis_X(short int* Sobel_X, char* format, const char* name, const int add, const int div, const unsigned int limit);
	void Sobel_axis_Y(short int* Sobel_Y, char* format, const char* name, const int add, const int div, const unsigned int limit);
	void filterSobel();
	
public:
	filterTools(const char* imgPath, const char* outName);
	~filterTools();
	
};