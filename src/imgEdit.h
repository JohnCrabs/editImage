//Constant Variables

#define DATA "output/data/"
#define OUTPUT_DATA "output/outData/"

#define ARRAY_SIZE  200
#define WORD_SIZE    30

/******************************************************************************************************/

//Trasform an integet into a string
extern void  my_itoa( int value, char* str, int base);

//Get a line from terminal
extern void myGetLine(char* line);

//Transform a text to lowerCase
extern void lowerCase(char* text);

//Transform a text to upperCase
extern void upperCase(char* text);

/******************************************************************************************************/

//Translate an Image to ERS Format
extern void GDAL_translateToERS(const char* imgPath, const char* ersFileName);

//Translate an ERS to IMG Format
extern void GDAL_translateERStoIMG(char* ersFileName, char* ImageName, char* ImageType);

//Create output Folder
extern void init_outputFolder();

/******************************************************************************************************/

//Read ERS File and keep the Image Size
extern void getImageSize_ERS(unsigned short int& height, unsigned short int& width, unsigned short int& bands, const char* name);

//Read BINARY PIXEL file and keep Pixel Values
extern void getPixelValues_IMG(const char* path, unsigned char* pixelTab, const int imgHeight, const int imgWidth, const int imgBands);

/******************************************************************************************************/

//Read a word from a file (skip blanks, new lines and special characters)
extern bool getWordFromFile(FILE* file, char* word);

/******************************************************************************************************/

//Create New ERS file
extern void createNewFile_ERS(const char* newName, const char* mainPath, const char* newPath,  const char* imgName, const int Height, const int Width, const int Bands);
