#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include <dirent.h>
#include <math.h>

#include "imgReduction.h"
#include "imgEdit.h"
#include "LSM_solReduction.h"

#include "graphicSystem.h"
#include "inputSystem.h"

#define COORD_WIN_X 100
#define COORD_WIN_Y 100

/******************************************************************************************************/

//Constructor
imgReduction::imgReduction(const char* path, const char* name)
{
	//Initialize IMG
	initIMG(path, name);
	initGraphicSystem();
	
	//Initialize IMG Window
	_window_img = createWin(_winWidth+COORD_WIN_X, _winHeight, winIMGname, SDL_WINDOW_OPENGL, 0, 0);
	_renderer_img = createRend(_window_img, -1, SDL_RENDERER_ACCELERATED, winIMGname);
	_texture_img = createTexture(_renderer_img, path);
	
	_fonts = TTF_OpenFont("fonts/open_sans/OpenSans_Regular.ttf", 16);
	if(_fonts == NULL) {
		printf("ERROR :: CANNOT OPEN FONTS!\n%s\n\n", TTF_GetError());
		exit(0);
	}
	
	initReductionSystem();
}

//Destructor
imgReduction::~imgReduction()
{
	clearStack();
}

/******************************************************************************************************/

//Crear the Stack
void imgReduction::clearStack()
{
	free(_imgPath);
	free(_ersFileName);
	free(_imgOutputFolder);
	
	free(_mainFilePath);
	
	free(_pixelTab);
	
	SDL_DestroyTexture(_texture_img);
	SDL_DestroyRenderer(_renderer_img);
	SDL_DestroyWindow(_window_img);
	
	SDL_DestroyTexture(_Coords_Message);
	
	TTF_CloseFont(_fonts);
	
	SDL_Quit();
}

/******************************************************************************************************/


//Initialize IMG
void imgReduction::initIMG(const char* path, const char* name)
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
	
	//Check for the existence of the directory "reductionFold"
	DIR* dir;
	dir = opendir("output/reductionFold");
	if(!dir) //If directory not exist create it
		system("mkdir output/reductionFold");
	closedir(dir); //Close dir
	
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
	
	//*********************************************//
	_winWidth = _imgWidth;
	_winHeight = _imgHeight;
	while(_winWidth > 1024 || _winHeight > 1024) {
		_winWidth /= 2;
		_winHeight /= 2;
	}
	
	//RECT IMG
	_rect_img.x = 0;
	_rect_img.y = 0;
	_rect_img.w = _winWidth; 
	_rect_img.h = _winHeight;
	
	//ZOOM RECT
	_zoom_rect.x = 0;
	_zoom_rect.y = 0;
	_zoom_rect.w = _imgWidth;
	_zoom_rect.h = _imgHeight;
}

//Clear Renderers
void imgReduction::clearRenderers()
{
	SDL_RenderClear(_renderer_img);
}

//Print Renderers
void imgReduction::updateRenderers()
{
	SDL_RenderCopy(_renderer_img, _texture_img, &_zoom_rect, &_rect_img);
	makeOtherWindow();
	SDL_SetRenderDrawColor(_renderer_img, 0, 0, 0, 255);
}

//Makes non main windows
void imgReduction::makeOtherWindow()
{
	SDL_SetRenderDrawColor(_renderer_img, 200, 200, 200, 255);
	_rect_coord.x = _winWidth;
	_rect_coord.y = 0;
	_rect_coord.w = COORD_WIN_X; 
	_rect_coord.h = COORD_WIN_Y;
	SDL_RenderFillRect(_renderer_img, &_rect_coord);
	makeCoordText();
}

//Write text
void imgReduction::makeCoordText()
{
	float buf;
	SDL_GetMouseState(&_zoom_num.ptX, &_zoom_num.ptY);
	buf = (_zoom_num.ptX*_zoom_num.scX );
	_Pix_coord_X = (int)buf - _winWidth/2; 
	buf = (_zoom_num.ptY*_zoom_num.scY );
	_Pix_coord_Y = (int)buf - _winHeight/2;
	
	char text[ARRAY_SIZE];
	
	//Coords:
	text[0] = '\0';
	strcat(text, "Coords:");
	
	_Text_Message = TTF_RenderText_Solid(_fonts, text, _Font_Color_Black);
	_Coords_Message = SDL_CreateTextureFromSurface(_renderer_img, _Text_Message);
	_rect_fonts.x = _winWidth + 5;
	_rect_fonts.y = 0;
	_rect_fonts.w = 80; 
	_rect_fonts.h = 20;
	SDL_RenderCopy(_renderer_img, _Coords_Message, NULL, &_rect_fonts);
	
	//X:
	text[0] = '\0';
	strcat(text, "X:");
	
	_Text_Message = TTF_RenderText_Solid(_fonts, text, _Font_Color_Black);
	_Coords_Message = SDL_CreateTextureFromSurface(_renderer_img, _Text_Message);
	_rect_fonts.x = _winWidth + 5;
	_rect_fonts.y = 30;
	_rect_fonts.w = 20; 
	_rect_fonts.h = 20;
	SDL_RenderCopy(_renderer_img, _Coords_Message, NULL, &_rect_fonts);
	
	//Mouse Motion X:
	text[0] = '\0';
	my_itoa(_Pix_coord_X, text, 10);
	
	_Text_Message = TTF_RenderText_Solid(_fonts, text, _Font_Color_Black);
	_Coords_Message = SDL_CreateTextureFromSurface(_renderer_img, _Text_Message);
	_rect_fonts.x = _winWidth + 35;
	_rect_fonts.y = 30;
	_rect_fonts.w = 40; 
	_rect_fonts.h = 30;
	SDL_RenderCopy(_renderer_img, _Coords_Message, NULL, &_rect_fonts);
	
	//Y:
	text[0] = '\0';
	strcat(text, "Y:");
	
	_Text_Message = TTF_RenderText_Solid(_fonts, text, _Font_Color_Black);
	_Coords_Message = SDL_CreateTextureFromSurface(_renderer_img, _Text_Message);
	_rect_fonts.x = _winWidth + 5;
	_rect_fonts.y = 60;
	_rect_fonts.w = 20; 
	_rect_fonts.h = 20;
	SDL_RenderCopy(_renderer_img, _Coords_Message, NULL, &_rect_fonts);
	
	//Mouse Motion Y:
	text[0] = '\0';
	my_itoa(_Pix_coord_Y, text, 10);
	
	_Text_Message = TTF_RenderText_Solid(_fonts, text, _Font_Color_Black);
	_Coords_Message = SDL_CreateTextureFromSurface(_renderer_img, _Text_Message);
	_rect_fonts.x = _winWidth + 35;
	_rect_fonts.y = 60;
	_rect_fonts.w = 40; 
	_rect_fonts.h = 30;
	SDL_RenderCopy(_renderer_img, _Coords_Message, NULL, &_rect_fonts);
}

//Initiallize System
void imgReduction::initReductionSystem()
{
	bool quit = true;
	while(quit) {
		SDL_Delay(60);
		clearRenderers();
		updateRenderers();
		checkInput();
		
		SDL_RenderPresent(_renderer_img);
	}
}

//Check the input event and perform actions
void imgReduction::checkInput()
{
	Event_Value event = inputWindowEvent(_window_img);
	switch(event) {
		case RETURN:
			break;
			
		case ZOOM_IN: {
			//ZOOM RECT
			int pix_X = _zoom_num.ptX - _zoom_rect.x;
			int pix_Y = _zoom_num.ptY - _zoom_rect.y;
			int pix_W = _zoom_rect.w - _zoom_num.ptX;
			int pix_H = _zoom_rect.h - _zoom_num.ptX;
			
			if(pix_X > 100 && pix_Y > 100 && pix_W > 100 && pix_H > 100) {
				_zoom_rect.x = _zoom_num.ptX - pix_X/2;
				_zoom_rect.y = _zoom_num.ptY - pix_Y/2;
				_zoom_rect.w = _zoom_num.ptX + pix_W/2;
				_zoom_rect.h = _zoom_num.ptY + pix_H/2;
				
				_zoom_num.scX = ((float) (pix_W/2 - pix_X/2) / _rect_img.w) * (_imgWidth/_rect_img.w);
				_zoom_num.scY = ((float) (pix_H/2 - pix_Y/2) / _rect_img.h) * (_imgHeight/_rect_img.h);
			}
			
			break;
		}
		
		case ZOOM_OUT: {
			//ZOOM RECT
			int pix_X = _zoom_rect.x;
			int pix_Y = _zoom_rect.y;
			int pix_W = _zoom_rect.w;
			int pix_H = _zoom_rect.h;
			
			if(pix_X > 0 && pix_Y > 0 && pix_W < _imgWidth && pix_H < _imgHeight) {
				_zoom_rect.x = 0;
				_zoom_rect.y = 0;
				_zoom_rect.w = _imgWidth;
				_zoom_rect.h = _imgHeight;
			}
			
			_zoom_num.scX = ((_zoom_rect.w - _zoom_rect.x) / _rect_img.w) / (_imgWidth/_winWidth);
			_zoom_num.scY = ((_zoom_rect.h - _zoom_rect.y) / _rect_img.h) / (_imgHeight/_winHeight);
			
			break;
		}
		
		case L_CLICK: {
			createPoint();
			break;
		}
		
		case ACTION: {
			exportPoint();
			break;
		}
		
		case COMMAND_LINE: {
			commandLine();
			break;
		}
		
		
	}
}

//Create Points
void imgReduction::createPoint()
{
	char buf[100];
	
	_sol_points[index].id = index;
	
	_sol_points[index].x = _Pix_coord_X;
	_sol_points[index].y = _Pix_coord_Y;
	
	printf("\nGive Real Coordinate X: ");
	myGetLine(buf);
	_sol_points[index].X = atof(buf);
	
	printf("\nGive Real Coordinate Y: ");
	myGetLine(buf);
	_sol_points[index].Y = atof(buf);
	
	printf("\nSpecify Point Use (type t -> tie point or c -> check point)\nType: ");
	myGetLine(buf);
	if(buf[0] == 't' ||  buf[0] == 'c')
		_sol_points[index].type = buf[0];
	else
		_sol_points[index].type = 't';
	
	index++;
}

//Export Points
void imgReduction::exportPoint()
{
	FILE* openFile;
	openFile = fopen("output/reductionFold/POINTS.rdc", "w");
	for(int i = 0; i < index; i++)
		//id pX pY X Y type
		fprintf(openFile, "%d %f %f %f %f %c\n", i, _sol_points[i].x, _sol_points[i].y, _sol_points[i].X, _sol_points[i].Y, _sol_points[i].type);
	fprintf(openFile, "\n");
	fclose(openFile);
	
	printf("\n\nREPORT :: EXPORT FILE SUCCESFULL!!\n\n");
}

//Import Points
void imgReduction::importPoint()
{
	char path[100];
	index = 0;
	printf("Type the path of Point Import File\n");
	printf("path/:$ ");
	myGetLine(path);
	
	FILE* openFile;
	openFile = fopen(path, "r");
	if (openFile == NULL) {
		printf("\nERROR :: FILE CANNOT OPEN!\n\n");
		return ;
	}
	int c;
	while((c=fgetc(openFile)) != EOF)
		if(c == '\n')
			index++;
	fclose(openFile);
	openFile = fopen(path, "r");
	//printf("\n\ni = %d", index);
	for(int i = 0; i < index; i++)
		fscanf(openFile, "%d %f %f %f %f %c\n", &_sol_points[i].id, &_sol_points[i].x, &_sol_points[i].y, &_sol_points[i].X, &_sol_points[i].Y, &_sol_points[i].type);      
	fclose(openFile);
	
	--index;
	printf("\n\nREPORT :: IMPORT FILE SUCCESFULL!\n");
	printf("Points: (id, pX, pY, X, Y, type)\n");
	for(int i = 0; i < index; i++)
		printf("%d %f %f %f %f %c\n", _sol_points[i].id, _sol_points[i].x, _sol_points[i].y, _sol_points[i].X, _sol_points[i].Y, _sol_points[i].type);
	printf("\n");
}

//Command Line
void imgReduction::commandLine()
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
void imgReduction::choices()
{
	int i = 0;
	clearScreen();
	printf("%d)Return to Image Window (type: return)\n", i++);
	printf("%d)Import Reduaction Points - file *.rdc (type: importRDC)\n", i++);
	printf("%d)Perform Reduction - Find coefficients (type: reduct)\n", i++);
	printf("%d)Import coefficients *.csv for reduct xy -> XY (type: x2X)\n", i++);
	printf("%d)Import coefficients *.csv for reduct XY -> xy (type: X2x)\n", i++);
	printf("%d)Create the new image (type: create)\n", i++);
	printf("\nTYPE: ");
}

//Clear Screen
void imgReduction::clearScreen()
{
	for(int i = 0; i <= 100; i++)
		printf("\n");
}

//Check Choice and Perform the actions
bool imgReduction::checkChoice(char* choice)
{
	if(strcmp(choice, "return") == 0)
		return false;
	
	else if(strcmp(choice, "importRDC") == 0)
		importPoint();
	
	else if(strcmp(choice, "reduct") == 0)
		calculateReductTables();
	
	else if(strcmp(choice, "x2X") == 0) {
		int cnt = 0;
		importCSV(_reduct.tab_a, cnt);
		if(cnt != 8)
			printf("WARNING :: NUMBER OF REDUCTION COEFFITIENTS != 8!\n\n");
	}
	
	else if(strcmp(choice, "X2x") == 0) {
		int cnt = 0;
		importCSV(_reduct.tab_A, cnt);
		if(cnt != 8)
			printf("WARNING :: NUMBER OF REDUCTION COEFFITIENTS != 8!\n\n");
	}
	
	else if(strcmp(choice, "create") == 0) {
		reductIMG_andPrint();
	}
	
	else
		printf("Uknown Command!\n\n");
	
	return true;
}

//Solve LSM and calculate tab_a and tab_A
void imgReduction::calculateReductTables()
{
	_reduct.tab_x = (double* )malloc(2*index*sizeof(double));
	_reduct.tab_X = (double* )malloc(2*index*sizeof(double));
	
	int t = 0;
	for (int i = 0; i < index; i++) {
		if(_sol_points[i].type == 't') {
			_reduct.tab_x[2*t] = (double)_sol_points[i].x;
			_reduct.tab_x[2*t + 1] = (double)_sol_points[i].y;
			
			_reduct.tab_X[2*t] = (double)_sol_points[i].X;
			_reduct.tab_X[2*t + 1] = (double)_sol_points[i].Y;
			
			t++;
		}
	}
	if (t < 4) {
		printf("ERROR :: TOO FEW POINTS!\n");
		return ;
	}
	else if (t == 4) {
		printf("WARNING :: NOT ENOUGH POINTS TO CALCULATE RMS!\n");
		printf("Do you want to proceed? (y/n)\n/$ ");
		char buf[100];
		myGetLine(buf);
		if(buf[0] == 'n')
			return ;
	}
	
	//LSM_Reduction(_reduct.tab_x, _reduct.tab_X, _reduct.tab_a, t);
	//calc_A(_reduct.tab_a, _reduct.tab_A);
	double buffer_A[8];
	LSM_Reduction(_reduct.tab_x, _reduct.tab_X, buffer_A, _reduct.tab_A, t);
	LSM_Reduction(_reduct.tab_X, _reduct.tab_x, buffer_A, _reduct.tab_a, t);
	
	FILE* openFile;
	openFile = fopen("output/reductionFold/XY_TO_xy.csv", "w");
	for (int i = 0; i < 8; i++)
		fprintf(openFile, "%lf\n", _reduct.tab_A[i]);
	fclose(openFile);
	openFile = fopen("output/reductionFold/xy_TO_XY.csv", "w");
	for (int i = 0; i < 8; i++)
		fprintf(openFile, "%lf\n", _reduct.tab_a[i]);
	fclose(openFile);
	
	double error_X;
	double error_Y;
	double total_err;
	double new_X;
	double new_Y;
	
	openFile = fopen("output/reductionFold/RMS.csv", "w");
	fprintf(openFile, "pixel_x;pixel_y;real_X;real_Y;reduct_X;reduct_Y;RMS_X;RMS_Y;TOTAL_RMS\n");
	for(int i = 0; i < index; i++) {
		calculate_new_X(new_X, new_Y, _reduct.tab_A, _sol_points[i].x, _sol_points[i].y);
		error_X = _sol_points[i].X - new_X;
		error_Y = _sol_points[i].Y - new_Y;
		total_err = sqrt(error_X*error_X + error_Y*error_Y);
		fprintf(openFile, "%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.7lf;%.7lf;%.7lf\n", _sol_points[i].x, _sol_points[i].y, _sol_points[i].X, _sol_points[i].Y, new_X, new_Y, error_X, error_Y, total_err);
	}
	fprintf(openFile, "\n\n");
	fprintf(openFile, "real_X;real_X;real_pix_x;real_pix_y;reduct_pix_x;reduct_pix_y;RMS_x;RMS_y;TOTAL_RMS\n");
	for(int i = 0; i < index; i++) {
		calculate_new_X(new_X, new_Y, _reduct.tab_a, _sol_points[i].X, _sol_points[i].Y);
		error_X = _sol_points[i].x - new_X;
		error_Y = _sol_points[i].y - new_Y;
		total_err = sqrt(error_X*error_X + error_Y*error_Y);
		fprintf(openFile, "%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.7lf;%.7lf;%.7lf\n", _sol_points[i].X, _sol_points[i].Y, _sol_points[i].x, _sol_points[i].y, new_X, new_Y, error_X, error_Y, total_err);
	}
	fclose(openFile);
}

//Calculate new X
void imgReduction::calculate_new_X(double &X, double &Y, const double* tab_A, const double x, const double y)
{
	double A, B, C;
	A = tab_A[0]*x + tab_A[1]*y + tab_A[2];
	B = tab_A[3]*x + tab_A[4]*y + tab_A[5];
	C = tab_A[6]*x + tab_A[7]*y + 1;
	
	X = A/C;
	Y = B/C;
}

//Import CSV Formats
void imgReduction::importCSV(double* tab, int &lines)
{
	char path[500];
	printf("Type the path of CSV file!\nType:");
	myGetLine(path);
	FILE* openFile;
	openFile = fopen(path, "r");
	if(openFile == NULL) {
		printf("\n\nERROR :: FILE CANNOT OPEN!\n\n");
		return ;
	}
	char c;
	while((c = fgetc(openFile)) != EOF)
		if(c == '\n')
			lines++;
	fclose(openFile);
	openFile = fopen(path, "r");
	for (int i = 0; i < lines; i++) {
		float flo;
		fscanf(openFile, "%f\n", &flo);
		tab[i] = flo;
	}
	fclose(openFile);
	
	printf("CSV:\n");
	for (int i = 0; i < lines; i++)
		printf("%f\n", tab[i]);
}

//Calculate table A
void imgReduction::calc_A(const double* tab_a, double* tab_A)
{
	/*
	 * tab_a[0] a11
	 * tab_a[1] a12
	 * tab_a[2] a13
	 * tab_a[3] a21
	 * tab_a[4] a22
	 * tab_a[5] a23
	 * tab_a[6] a31
	 * tab_a[7] a32
	 */
	
	double div = tab_a[0]*tab_a[4]-tab_a[3]*tab_a[1];
	
	tab_A[0] =  (tab_a[4]-tab_a[5]*tab_a[7])/div;
	tab_A[1] = -(tab_a[1]-tab_a[2]*tab_a[7])/div; 
	tab_A[2] =  (tab_a[1]*tab_a[5]-tab_a[2]*tab_a[4])/div;
	tab_A[3] = -(tab_a[3]-tab_a[5]*tab_a[6])/div;
	tab_A[4] =  (tab_a[0]-tab_a[2]*tab_a[6])/div;
	tab_A[5] = -(tab_a[0]*tab_a[5]-tab_a[2]*tab_a[3])/div;
	tab_A[6] =  (tab_a[3]*tab_a[7]-tab_a[4]*tab_a[6])/div;
	tab_A[7] = -(tab_a[0]*tab_a[7]-tab_a[1]*tab_a[6])/div;
}

//Perform reduction 
void imgReduction::reductIMG_andPrint()
{
	int newIMG_W;
	int newIMG_H;
	int scale;
	double x_min_1, y_min_1, x_max_1, y_max_1;
	double x_min_2, y_min_2, x_max_2, y_max_2;
	double step;
	char buffer[100];
	
	double WID;
	double HGHT;
	
	WID = _winWidth/2;
	HGHT = _winHeight/2;
	
	//Find x_min_1, y_min_1
	calculate_new_X(x_min_1, y_max_1, _reduct.tab_A, -WID, -HGHT); //(x_min, y_max)->(-WID, -HGHT)
	//Find x_max_1, y_max_1
	calculate_new_X(x_max_1, y_max_2, _reduct.tab_A, WID, -HGHT); //(x_max, y_max)->(WID, -HGHT)
	
	//Find x_min_2, y_min_2
	calculate_new_X(x_max_2, y_min_1, _reduct.tab_A, WID, HGHT); //(x_max, y_min)->(WID, HGHT)
	//Find x_max_2, y_max_2
	calculate_new_X(x_min_2, y_min_2, _reduct.tab_A, -WID, HGHT); //(x_min, y_min)->(-WID, HGHT)
	
	//User insert the scale
	printf("\n\n SCALE = ");
	myGetLine(buffer);
	scale = atoi(buffer);
	
	//calculate newIMG_W, newIMG_H
	step = (double)scale*0.0001; //[m]
	newIMG_W = 0; //[pixel]
	newIMG_H = 0; //[pixel]
	
	
	/*************************************************/
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_REDUCT_IMG");
	//Create New Output File
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//printf("\n\nnewIMG_H = %d\nnewIMG_W = %d", newIMG_H, newIMG_W);
	
	
	
	short int pixelValue;
	double pix_X, pix_Y;
	FILE* outFile = fopen(pathNew, "w");
	for(double y = y_max_1; y > y_min_1; y-=step) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(double x = x_min_2; x < x_max_1; x+=step) {
				calculate_new_X(pix_X, pix_Y, _reduct.tab_a, x, y);
				pix_X = (pix_X + _winWidth/2)*(_imgWidth/_winWidth);
				pix_Y = (pix_Y + _winHeight/2)*(_imgHeight/_winHeight);
				if(((int)pix_X <= 0 || (int)pix_X >= _imgWidth) || ((int)pix_Y <= 0 || (int)pix_Y >= _imgHeight))
					pixelValue = 0;
				else {
					//nearest neightbour
					//pixelValue = _pixelTab[(((int)pix_Y*_imgBands) + bands)*_imgWidth + (int)pix_X];
					
					//cubic
					int index = 0;
					for(int i = pix_Y; i < pix_Y + 1; i++)
						for(int j = pix_X; j < pix_X; j++) {
							pixelValue += _pixelTab[(((int)pix_Y*_imgBands) + bands)*_imgWidth + (int)pix_X];
							index++;
						}
					pixelValue /= (index+1);
				}
				fprintf(outFile, "%c", (char)pixelValue);
				if(newIMG_H == 0)
					newIMG_W++;
			}
		}
		newIMG_H++;
	}
	fclose(outFile);
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, newIMG_H, newIMG_W, _imgBands);
	
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
	
	free(pathNew);
	free(newImageName);
}

//Finds Minimum value
double imgReduction::findMin(double m1, double m2)
{
	if (m1 < m2)
		return m1;
	else
		return m2;
}

//Finds Minimum value
double imgReduction::findMax(double m1, double m2)
{
	if (m1 < m2)
		return m2;
	else
		return m1;
}