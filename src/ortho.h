#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

struct IMGpar
{
	double c;
	double xo;
	double yo;
	
	double Xo;
	double Yo;
	double Zo;
	
	double w;
	double f;
	double k;
};

struct DEM
{
	int rows;
	int cols;
	
	double* dem;
};

struct XYZ
{
	int size;
	
	double *X;
	double *Y;
	double *Z;
};

struct AFFpar
{
	double a11;
	double a12;
	double a13;
	
	double a21;
	double a22;
	double a23;
};

struct Rtab
{
	double a11;
	double a12;
	double a13;
	
	double a21;
	double a22;
	double a23;
	
	double a31;
	double a32;
	double a33;
};

class ortho
{
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
	
	//importDEM
	double step;
	DEM _dem;
	XYZ _xyz;
	IMGpar _IMGpar;
	AFFpar _AFFpar;
	Rtab R;
	
	/******************************************************************************************************/
	void initIMG(const char* path, const char* name);
	void runOrtho();
	void commandLine();
	void choices();
	void clearScreen();
	bool checkChoice(char* choice);
	/******************************************************************************************************/
	void importDEM();
	void importXYZ();
	void importIMGpar();
	void importAFFpar();
	void readNum(FILE* file, double* val);
	
	void orthoDEM();
	void orthoXYZ();
	void linearInterpolationXYZ(double X, double Y, double *Z);
	void collinearity(double X, double Y, double Z, double *x, double *y);
	void affine(double x, double y, int *i, int *j);
	/******************************************************************************************************/
	
	void clearMemory();
	
	/******************************************************************************************************/
	
public:
	ortho(const char* path, const char* name);
	~ortho();
};