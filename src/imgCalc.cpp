//Libraries from C
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>

//Other Libraries
#include <dirent.h>

#include "imgEdit.h"
#include "imgCalc.h"

const int PATH_SIZE = 100;
const int NAME_SIZE =  20;

/******************************************************************************************************/

//Constructor
imgCalc::imgCalc(const char* path, const char* name)
{
	initCalc(path, name);
}


/******************************************************************************************************/

//Initialize Calculator
void imgCalc::initCalc(const char* path, const char* name)
{
	//Create and Read input file
	FILE* openFile;
	openFile = fopen(path, "r");
	//Check if file EXISTS
	if(openFile == 0) {
		printf("ERROR: TXT FILE NOT EXIST OF WRONG PATH!\n\n");
		exit(0);
	}
	char c;
	//Find the lines of the file
	while ((c=fgetc(openFile)) != EOF)
		if(c == ';')
			_lines++;
	//Check if the file has correct number of paths
	if(_lines == 1) {
		printf("ERROR: TOO FEW PATHS!\n\n");
		exit(0);
	}
	fclose(openFile);
	
	//Read File and save the paths
	openFile = fopen(path, "r");
	for(int i = 0; i < _lines; i++) {
		
		char src[5];
		_imgName[i][0] = '\0';
		strcat(_imgName[i], "img");
		my_itoa(i+1, src, 10);
		strcat(_imgName[i], src);
		
		while((c=fgetc(openFile)) != '\n' && c != EOF) {
			if(c == '"') {
				int j = 0;
				while((c=fgetc(openFile)) != '"') {
					if(c == '\n' || c == EOF) {
						printf("ERROR: FILE FORMAT IS NOT CORRECT!\n\n");
						exit(0);
					}
					else
					_imgPath[i][j++] = c;
					
				}
				_imgPath[i][j] = '\0';
			}
			
			if(c == '<') {
				int j = 0;
				while((c=fgetc(openFile)) != '>') {
					if(c == '\n' || c == EOF) {
						printf("ERROR: FILE FORMAT IS NOT CORRECT!\n\n");
						exit(0);
					}
					else
						_imgName[i][j++] = c;
				}
				_imgName[i][j] = '\0';
			}
		}
	}
	fclose(openFile);
	
	
	
	printIMG();
}

void imgCalc::printIMG()
{
	
	for (int i = 0; i < _lines; i++)
		printf("\n*Path :: %s\n*Name :: %s\n", _imgPath[i], _imgName[i]);
}