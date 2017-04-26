//Libraries from C
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

//Other Libraries
#include <dirent.h>

//Header File
#include "imgEdit.h"


/******************************************************************************************************/

//Trasform an integet into a string
void my_itoa(int value, char* str, int base) {
	char bufer[1024];
	int i = 0;
	if(value < 0) {
		bufer[i++] = '-';
		value *= -1;
	}
	if(value == 0)
		bufer[i++] = '0';
	while (value > 0) {
		if(((value%base) + '0') > '9')
			bufer[i++] = ((value%base) + '0' - ('9' + 1)) + 'A';
		else
			bufer[i++] = (value%base) + '0';
		value /= base;
	}
	bufer[i--] = '\0';
	int j;
	for(j = 0; i >= 0; j++, i--)
		str[j] = bufer[i];
	str[j] = '\0';
}

//Get a line from terminal
void myGetLine(char* line)
{
	while((*line=getchar()) != '\n')
		line++;
	*line = '\0';
}

//Transform a text to lowerCase
void lowerCase(char* text)
{
	for(int i = 0; i < (int)strlen(text); i++)
		if(isalpha(text[i]))
			text[i] = tolower(text[i]);
}

//Transform a text to upperCase
void upperCase(char* text)
{
	for(int i = 0; i < (int)strlen(text); i++)
		if(isalpha(text[i]))
			text[i] = toupper(text[i]);
}

/******************************************************************************************************/

//Translate an Image to ERS Format
void GDAL_translateToERS(const char* imgPath, const char* ersFileName)
{
	//Create and Allocate memory for "commandGDAL"
	char commandGDAL[ARRAY_SIZE] = "gdal_translate -of ERS ";
	
	//Create the GDL Command
	strcat(commandGDAL, imgPath);
	strcat(commandGDAL, " ");
	strcat(commandGDAL, DATA);
	strcat(commandGDAL, ersFileName);
	strcat(commandGDAL, ".ers");
	
	//Check for the existence of the directory "output"
	DIR* dir = opendir("output");
	if(!dir) //If directory not exist create it
		system("mkdir output");
	closedir(dir); //Close dir
	
	//Check for the existence of the directory "data"
	dir = opendir("output/data");
	if(!dir) //If directory not exist create it
		system("mkdir output/data");
	closedir(dir); //Close dir
	
	init_outputFolder();
	
	//Run commandGDAL
	printf("\n%s\n", commandGDAL);
	system(commandGDAL);
	printf("\n");
}

//Translate ERS File to Image
void GDAL_translateERStoIMG(char* ersFileName, char* ImageName, char* ImageType)
{
	//create gdal command
	char commandGDAL[ARRAY_SIZE] = "gdal_translate -of ";
	
	//complete GDAL command with arguments
	strcat(commandGDAL, ImageType);
	strcat(commandGDAL, " ");
	strcat(commandGDAL, ersFileName);
	strcat(commandGDAL, " ");
	strcat(commandGDAL, ImageName);
	
	//run the commant from the system
	printf("\n%s\n", commandGDAL);
	system(commandGDAL);
	printf("\n");
}

//Create output Folder
void init_outputFolder()
{
	
	//Check for the existence of the directory "outData"
	DIR* dir = opendir(OUTPUT_DATA);
	//If directory not exist create it
	if(!dir) {
		char command[50] = "mkdir ";
		strcat(command, OUTPUT_DATA);
		system(command);
	}
	closedir(dir); //Close dir
}

/******************************************************************************************************/

//Read ERS File and keep the Image Size
void getImageSize_ERS(unsigned short int& height, unsigned short int& width, unsigned short int& bands, const char* name)
{
	//Allocate memory
	char inputPath[100];
	
	//Create the Input Path
	inputPath[0] = '\0';
	strcat(inputPath, "output/data/");
	strcat(inputPath, name);
	strcat(inputPath, ".ers");
	
	char word[30];
	
	FILE *file = fopen(inputPath, "r");
	while(getWordFromFile(file, word)) {
		//find the height of picture = Ypixels
		if(strcmp(word, "NrOfLines") == 0) {
			getWordFromFile(file, word);
			height = atoi(word);
		}
		
		//find the width of picture = Xpixels
		else if(strcmp(word, "NrOfCellsPerLine") == 0) {
			getWordFromFile(file, word);
			width = atoi(word);
		}
		
		//find the channels of picture
		else if(strcmp(word, "NrOfBands") == 0) {
			getWordFromFile(file, word);
			bands = atoi(word);
		}
	}
	fclose(file);
}

//Read BINARY PIXEL file and keep Pixel Values
void getPixelValues_IMG(const char* path, unsigned char* pixelTab, const int imgHeight, const int imgWidth, const int imgBands)
{
	//Open file
	FILE* file = fopen(path, "r");
	//Error checking
	if(!file) {
		printf("\n\nERROR:FILE \"%s\" NOT EXIST!\n\n", path);
		exit(0);
	}
	//Write pixel value to table
	fread(pixelTab, sizeof(char), imgHeight*imgWidth*imgBands, file);
	//Close file
	fclose(file);
}

/******************************************************************************************************/

//Read a word from a file (skip blanks, new lines and special characters)
bool getWordFromFile(FILE* file, char* word)
{
	//not a word
	while (!isalnum(*word = fgetc(file)))
		if(*word == EOF)
			return false;
	//is a word
	word++;
	while(isalnum(*word = fgetc(file)))
		word++;
	*word = '\0';
	return true;
}

/******************************************************************************************************/

//Create New ERS file
void createNewFile_ERS(const char* newName, const char* mainPath, const char* newPath, const char* imgName, const int Height, const int Width, const int Bands)
{
	//NrOfLines = Height
	//NrOfCellsPerLine = Width
	//NrOfBands = Bands
	
	//Allocate Memory
	//Path to mainPath.ers file
	char* oldFile_ERS;
	oldFile_ERS = (char*)malloc(ARRAY_SIZE*sizeof(char));
	oldFile_ERS[0] = '\0';
	strcat(oldFile_ERS, mainPath);
	strcat(oldFile_ERS, ".ers");
	
	//Path to new.ers file
	char* newFile_ERS;
	newFile_ERS = (char*)malloc(ARRAY_SIZE*sizeof(char));
	newFile_ERS[0] = '\0';
	strcat(newFile_ERS, newPath);
	strcat(newFile_ERS, newName);
	strcat(newFile_ERS, ".ers");
	
	//New Word
	char* newWord;
	newWord = (char*)malloc(WORD_SIZE*sizeof(char));
	newWord[0] = '\0';
	//strcat(newWord, imgName);
	strcat(newWord, newName);
	
	//copy the ers prototype to output path
	char word[ARRAY_SIZE];
	FILE* inputFile = fopen(oldFile_ERS, "r");
	FILE* outputFile = fopen(newFile_ERS, "w");
	
	enum STATE {NONE, HEIGHT, WIDTH, BANDS};
	STATE state = STATE::NONE;
	
	char c;
	int i;
	while((c = fgetc(inputFile)) != EOF) {
		if(isalnum(c)) {
			
			if(state == STATE::HEIGHT) {
				while(isdigit(c = fgetc(inputFile)))
					;
				fprintf(outputFile, "%d", Height);
				state = STATE::NONE;
			}
			
			else if(state == STATE::WIDTH) {
				while(isdigit(c = fgetc(inputFile)))
					;
				fprintf(outputFile, "%d", Width);
				state = STATE::NONE;
			}
			
			else if(state == STATE::BANDS) {
				while(isdigit(c = fgetc(inputFile)))
					;
				fprintf(outputFile, "%d", Bands);
				state = STATE::NONE;
			}
			
			i = 0;
			word[i++] = c;
			while(isalnum((c = fgetc(inputFile))))
				word[i++] = c;
			word[i] = '\0';
			if(strcmp(word, imgName) == 0)
				fprintf(outputFile, "%s", newWord);
			else if (strcmp(word, "NrOfLines") == 0) {
				fprintf(outputFile, "%s", word);
				state = STATE::HEIGHT;
			}
			else if (strcmp(word, "NrOfCellsPerLine") == 0) {
				fprintf(outputFile, "%s", word);
				state = STATE::WIDTH;
			}
			else if(strcmp(word, "NrOfBands") == 0) {
				fprintf(outputFile, "%s", word);
				state = STATE::BANDS;
			}
			else
				fprintf(outputFile, "%s", word);
		}
		
		fprintf(outputFile, "%c", c);
	}
	
	fclose(inputFile);
	fclose(outputFile);
	
	//Deallocate Memory
	free(oldFile_ERS);
	free(newFile_ERS);
	free(newWord);
}

