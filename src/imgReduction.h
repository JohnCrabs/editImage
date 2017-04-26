#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


struct Zoom
{
	int x;
	int y;
	int w;
	int h;
	
	int ptX;
	int ptY;
	
	float scX;
	float scY;
};

struct Points
{
	int id;
	
	float x;
	float y;
	
	float X;
	float Y;
	
	int pX;
	int pY;
	
	char type;
};

struct ReductTables
{
	double* tab_x;
	double* tab_X;
	
	double tab_a[8];
	double tab_A[8];
};

class imgReduction
{
private:
	
	//variable for initialize IMG
	char* _imgPath;
	char* _ersFileName;
	char* _imgOutputFolder;
	char* _mainFilePath;
	
	unsigned short int _imgWidth;
	unsigned short int _imgHeight;
	unsigned short int _imgBands;
	
	unsigned short int _winWidth;
	unsigned short int _winHeight;
	
	char _data[20];
	
	//Image Graphic Window
	char winIMGname[50] = "Image Reduction Window";
	SDL_Window*   _window_img;
	SDL_Renderer* _renderer_img;
	SDL_Texture*  _texture_img;
	SDL_Rect _zoom_rect;
	SDL_Rect _rect_img;
	
	Zoom _zoom_num = {0,0,0,0,1,1,1,1};
	Points _sol_points[100];
	int index = 0;
	
	/*=================================================*/
	
	//Coord Graphic Rect variables
	//Font Variables
	TTF_Font* _fonts;
	SDL_Color _Font_Color_Black = {0,0,0,0};
	
	//Text Variables
	SDL_Surface* _Text_Message;
	SDL_Texture* _Coords_Message;
	
	//Keep coordinates
	int _Pix_coord_X;
	int _Pix_coord_Y;
	
	//Output Variables
	SDL_Rect _rect_coord;
	SDL_Rect _rect_fonts;
	
	
	//Reduct Tables
	ReductTables _reduct;
	int _red_index_a;
	int _red_index_A;
	
	//Image pixel values
	unsigned char* _pixelTab;
	
	/**************************************************/
	
	void clearStack();
	void initIMG(const char* path, const char* name);
	
	void clearRenderers();
	void updateRenderers();
	void makeOtherWindow();
	void makeCoordText();
	void initReductionSystem();
	void checkInput();
	void createPoint();
	void exportPoint();
	void importPoint();
	void commandLine();
	void choices();
	void clearScreen();
	bool checkChoice(char* choice);
	void calculateReductTables();
	void calculate_new_X(double &X, double &Y, const double* tab_A, const double x, const double y);
	void importCSV(double* tab, int &lines);
	void calc_A(const double* tab_a, double* tab_A);
	void reductIMG_andPrint();
	double findMin(double m1, double m2);
	double findMax(double m1, double m2);
	
public:
	imgReduction(const char* path, const char* name);
	~imgReduction();
};