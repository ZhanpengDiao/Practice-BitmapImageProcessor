/*******************************************************************************
*   Author: Zhanpeng Diao   u5788688            *
*   Last update: 12/7/2016
*
*   Program name: Image processing
*   This program uses a BMP file as an input file, processes this file by read
*   in data into memory, changes as user requires.
*
*   Attention: only 24 bpp BMP files are vaild.            *
*******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

typedef unsigned short WORD; // 2 bytes unit
typedef unsigned int DWORD; // 4 bytes unit

// For convinent use of informations of headers, I declared two structures
// to fit in
// BMP file header structure: general information of an file image (14 bytes)
#pragma pack(2) // bytes aligning
typedef struct bmpFileHeader {
    WORD type;
    DWORD size;
    WORD reserved1;
    WORD reserved2;
    DWORD offset;
} BMPFILE;
#pragma pack()

// BMP info header structure: detailed information of an file image (40 bytes)
typedef struct bmpInfoHeader
{
    DWORD infoSize;
    DWORD width; // signed integer
    DWORD height; // signed integer
    WORD planes; // must be 1
    WORD bitsPerPixel;
    DWORD compression;
    DWORD imageSize;
    DWORD XPixelsPerMeter; // signed integer
    DWORD YPixelsPerMeter; // signed integer
    DWORD colorUsed;
    DWORD importantColor;
} BMPINF;

// declaring global varibles
BMPFILE fileHeader; // BMP file header
BMPINF infoHeader; // BMP infomation header
unsigned char *imageData; // data of pixel arrays
char *outputName = "out.bmp"; // default output name
int lineSize; // the size of a single pixel line
char flag = 0b00000000; // the flag for switching functions
double colorScope; // the maxinium value of color in pixels (24 bpp should be 255)

/*******************************************************************************
*   this function reads in data of a images         *
*   including headers and pixel arrays          *
*   this function needs an file handler (pointer) as input parameter           *
*
*   the flag of this function is at position 7 (1 << 7);           *
*******************************************************************************/
void readInfo(FILE *file) {
    // switching check
    char checkFlag = 0b10000000; // position 0
    if((checkFlag & flag) == 0) return;

    // trasfering cursor to the start of a file
    fseek(file, 0, SEEK_SET); // SEEK_SET 0

    // storing file headers
    fread(&fileHeader, sizeof(fileHeader), 1, file);
    fread(&infoHeader, sizeof(infoHeader), 1, file);

    // printing out the width and height as required
    int fileWidth = infoHeader.width;
    int fileHeight = infoHeader.height;
    printf("Image width: %d\nImage height: %d\n", fileWidth, fileHeight);

    // allocating memory
    imageData = (unsigned char *) malloc(infoHeader.imageSize);

    // transfering cursor to the start of pixel arrays
    fseek(file, (long)fileHeader.offset, SEEK_SET);

    // storing pixel arrays
    fread(imageData, sizeof(unsigned char), infoHeader.imageSize, file);

    // assigning the size of a line
    lineSize = ((infoHeader.width * infoHeader.bitsPerPixel + 31) / 32) * 4;

    // assigning color scope
    colorScope = (pow(2, (infoHeader.bitsPerPixel / 3)) - 1);
}

/*******************************************************************************
*   this function keeps image data and saves as a BMP file with a given
*   name (-o) or default('out.cmp') name          *
*   this function requires a file handler (pointer) as input parameter         *
*
*   the command line argument option of this function is '-o'            *
*
*   the flag of this function is at position 0 (1 << 0);            *
*******************************************************************************/
void saveImage(FILE *file) {
    // switching check
    char checkFlag = 0b00000001; // position 0
    if((checkFlag & flag) == 0) return;

    // creating file, if exists, truncate to 0 in length
    file = fopen(outputName, "wb+");

    // writing file header - image type - 2 bytes
    fseek(file, 0, SEEK_SET);
    fwrite(&(fileHeader.type), sizeof(fileHeader.type), 1, file);

    // writing file header - size of file - 4 bytes
    fseek(file, 2, SEEK_SET);
    fwrite(&(fileHeader.size), sizeof(fileHeader.size), 1, file);

    // writing file header - reserved space 1 - 2 bytes
    fseek(file, 6, SEEK_SET);
    fwrite(&(fileHeader.reserved1), sizeof(fileHeader.reserved1), 1, file);

    // writing file header - reserved space 2 - 2 bytes
    fseek(file, 8, SEEK_SET);
    fwrite(&(fileHeader.reserved2), sizeof(fileHeader.reserved2), 1, file);

    // writing file header - offset - 4 bytes
    fseek(file, 10, SEEK_SET);
    fwrite(&(fileHeader.offset), sizeof(fileHeader.offset), 1, file);

    // writing info header - size of info header - 4 bytes
    fseek(file, 14, SEEK_SET);
    fwrite(&(infoHeader.infoSize), sizeof(infoHeader.infoSize), 1, file);

    // writing info header - width of image - 4 bytes
    fseek(file, 18, SEEK_SET);
    fwrite(&(infoHeader.width), sizeof(infoHeader.width), 1, file);

    // writing info header - height of image - 4 bytes
    fseek(file, 22, SEEK_SET);
    fwrite(&(infoHeader.height), sizeof(infoHeader.height), 1, file);

    // writing info header - number of color planes - 2 bytes
    fseek(file, 26, SEEK_SET);
    fwrite(&(infoHeader.planes), sizeof(infoHeader.planes), 1, file);

    // writing info header - size of info header - 2 bytes
    fseek(file, 28, SEEK_SET);
    fwrite(&(infoHeader.bitsPerPixel), sizeof(infoHeader.bitsPerPixel), 1, file);

    // writing info header - compression type - 4 bytes
    fseek(file, 30, SEEK_SET);
    fwrite(&(infoHeader.compression), sizeof(infoHeader.compression), 1, file);

    // writing info header - image size - 4 bytes
    fseek(file, 34, SEEK_SET);
    fwrite(&(infoHeader.imageSize), sizeof(infoHeader.imageSize), 1, file);

    // writing info header - horizontal resolutions - 4 bytes
    fseek(file, 38, SEEK_SET);
    fwrite(&(infoHeader.XPixelsPerMeter), sizeof(infoHeader.XPixelsPerMeter), 1, file);

    // writing info header - vertical resolutions - 4 bytes
    fseek(file, 42, SEEK_SET);
    fwrite(&(infoHeader.YPixelsPerMeter), sizeof(infoHeader.YPixelsPerMeter), 1, file);

    // writing info header - colors in palette - 4 bytes
    fseek(file, 46, SEEK_SET);
    fwrite(&(infoHeader.colorUsed), sizeof(infoHeader.colorUsed), 1, file);

    // writing info header - important color used - 4 bytes
    fseek(file, 50, SEEK_SET);
    fwrite(&(infoHeader.importantColor), sizeof(infoHeader.importantColor), 1, file);

    // writing image data
    fseek(file, 54, SEEK_SET);
    fwrite(imageData, sizeof(unsigned char), infoHeader.imageSize, file);

    // close file
    fclose(file);
}

/*******************************************************************************
*   this function is for displaying usage message         *
*   the command line argument of this function is -h
*
*   the flag of this function is at position 2 (1 << 2);            *
*******************************************************************************/
void displayUsageMessage() {
    char checkFlag = 0b00000100; // position 2
    if((checkFlag & flag) == 0) return;

    printf("\n");
    printf("Usage: bmpedit [OPTIONS...] [input.bmp]\n\n");
    printf("DESCRIPTION:\n");
    printf("\tThis program does simple edits of BMP image files. When the program \n");
    printf("\truns it first prints out the width and the height of the input image \n");
    printf("\twithin the BMP file. Once this is done if there is a filter (or sequence of filters)\n");
    printf("\tthen they are applied to the image. The resulting image is also\n");
    printf("\tstored using BMP format into an output file.  Without any filters\n");
    printf("\tonly the width and height of the image is output.\n\n");
    printf("OPTIONS:\n");
    printf("\t-o FILE      Sets the output file for modified images (default output\n");
    printf("\t             file is \"out.bmp\").\n\n");
    printf("\t-t 0.0-1.0   Apply a threshold filter to the image with a threshold\n");
    printf("\t             the threshold value given.\n\n");
    printf("\t-h           Displays this usage message.\n\n");
    printf("\t-c [width start],[width end]:[height start],[height end]\n");
    printf("\t             Crops a specific part of a image，input fractions in proportion.\n");
    printf("\t             e.g. 0.0,1.0:0.0,1.0 returns the original image.\n\n");
    printf("\t-l -1.0-1.0  Change the level of brightness of an image.\n\n");
    printf("\t-b           Change a colurful image to white-and-black.\n\n");
}

/*******************************************************************************
*   this function is for the part2 of this assignment          *
*   this function perform a threshold on an image, setting all pixels to white *
*   which are greater than threshold value.             *
*
*   the command argument option of this function is '-t'        *
*
*   the flag of this function is at position 1 (1 << 1);        *
*******************************************************************************/
void thresholdFilter(double thresholdValue) {
    // switching check
    char checkFlag = 0b00000010; // position 1
    if((checkFlag & flag) == 0) return;

    // check if the threshold value is vaild
    if(thresholdValue > 1.0 || thresholdValue < 0.0) {
        fprintf(stderr, "*Input Error: Threshold value should be a number between 0.0 and 1.0.\n");
        fprintf(stderr, "*Please check help document (-h).\n");
        fprintf(stderr, "*Your Input: %g\n", thresholdValue);
        exit(1);
    }

    int i, j;
    // averageIndex: the value which is used to compare with threshold value
    double averageIndex;

    for(i = 0; i < infoHeader.height; i ++) {
        for(j = 0; j < lineSize - 3; j += 3) {
            // calculating average value
            averageIndex = ((*(imageData + i * lineSize + j) +\
                       *(imageData + i * lineSize + j + 1) +\
                       *(imageData + i * lineSize + j + 2)) / 3.0)\
                       / colorScope;
            // brighter to white, darker to dark
            if(averageIndex > thresholdValue) {
                *(imageData + i * lineSize + j) = colorScope;
                *(imageData + i * lineSize + j + 1) = colorScope;
                *(imageData + i * lineSize + j + 2) = colorScope;
            }
            else {
                *(imageData + i * lineSize + j) = 0;
                *(imageData + i * lineSize + j + 1) = 0;
                *(imageData + i * lineSize + j + 2) = 0;
            }
        }
    }
}

/*******************************************************************************
*   this is an optional extension made for the part4 of this assignment        *
*   this function crops image to a given size(width, height), it requires four  *
*   fraction numbers as input:
*   1. the start position of width in proportion          *
*   2. the end position of width in proportion            *
*   3. the start position of height in proportion           *
*   4. the end position of height in proportion             *
*
*   the command argument option of this function is '-c'        *
*
*   the flag of this function is at position 3 (1 << 3);        *
*******************************************************************************/
void cropImageExtension(double newWidthStartProp, double newWidthEndProp, double newHeightStartProp, double newHeightEndProp) {
    // switching check
    char checkFlag = 0b00001000; // position 3
    if((checkFlag & flag) == 0) return;

    int newWidthStart  = (int)(newWidthStartProp * infoHeader.width);
    int newWidthEnd    = (int)(newWidthEndProp * infoHeader.width);
    int newHeightStart = (int)(newHeightStartProp * infoHeader.height);
    int newHeightEnd   = (int)(newHeightEndProp * infoHeader.height);

    // validity check
    if(newWidthStart < 0 || newWidthEnd > infoHeader.width ||\
       newHeightStart < 0 || newHeightEnd > infoHeader.height ||\
       newWidthEnd < newWidthStart ||\
       newHeightEnd < newHeightStart) {
        fprintf(stderr, "*Input Error: New width and Height of the image shoud be fractions which are\n ");
        fprintf(stderr, "              greater than 0 and less than 1 simtaneously.\n");
        fprintf(stderr, "*Please check help document (-h).\n");
        fprintf(stderr, "*Your Input: \n");
        fprintf(stderr, "*Start position of width: %d\n", newWidthStart);
        fprintf(stderr, "*End position of width: %d\n", newWidthEnd);
        fprintf(stderr, "*Start position of height: %d\n", newHeightStart);
        fprintf(stderr, "*End position of height: %d\n", newHeightEnd);
        exit(1);
    }

    // if new width and height both equal to their original value respectively, quitting directly
    if(newWidthStart == 0 && newHeightStart == 0 &&\
       newWidthEnd == infoHeader.width && newHeightEnd == infoHeader.height) {
        return;
    }

    // assigning new width and height
    infoHeader.width  = newWidthEnd - newWidthStart;
    infoHeader.height = newHeightEnd- newHeightStart;

    // assigning new file size
    int newLineSize = ((infoHeader.width * infoHeader.bitsPerPixel + 31)/ 32) * 4;
    infoHeader.imageSize = newLineSize * infoHeader.height;
    fileHeader.size = infoHeader.imageSize + sizeof(infoHeader) + sizeof(fileHeader);

    //processing image data
    unsigned char *imageDataTemp = (unsigned char *) malloc(infoHeader.imageSize);
    int i, j;
    for(i = 0; i < infoHeader.height; i ++) {
        for(j = 0; j < newLineSize; j ++) {
            imageDataTemp[i * newLineSize + j] =\
                imageData[(i + newHeightStart) * lineSize + (j + newWidthStart)];
        }
    }
    imageData = (unsigned char *) realloc(imageData, infoHeader.imageSize);
    for(i = 0; i < infoHeader.height; i ++) {
        for(j = 0; j < newLineSize; j ++) {
            imageData[i * newLineSize + j] = imageDataTemp[i * newLineSize + j];
        }
    }
    lineSize = newLineSize;

    free(imageDataTemp);
}

/*******************************************************************************
*   this is an optional extension made for the part4 of this assignment        *
*   this function increses or reduces the brightness of a image, it requires   *
*   a fraction number as input (-1.0-1.0), it adds the given level to     *
*   every color byte, if the value of a color byte adds the brightness level   *
*   is greater than 255, it equals to 255; if less than 0, it equals to 0.     *
*
*   the command argument option of this function is '-l'        *
*
*   the flag of this function is at position 4 (1 << 4);        *
*******************************************************************************/
void setBrightnessExtension(double coefBrightness) {
    // switching check
    char checkFlag = 0b00010000; // position 4
    if((checkFlag & flag) == 0) return;

    //set the level equals to maxinium value of color multiples coefficient of brightness.
    int level = (int)(coefBrightness * colorScope);

    // check if the level of brightness is vaild
    if(level > colorScope || level < -1 * colorScope) {
        fprintf(stderr, "*Input Error: Brightness level should be a number between -1.0 and 1.0.\n");
        fprintf(stderr, "*Please check help document (-h).\n");
        fprintf(stderr, "*Your Input: %g\n", coefBrightness);
        exit(1);
    }

    int i, j;
    for(i = 0; i < infoHeader.height; i ++) {
        for(j = 0; j < lineSize; j += 1) {

            // add/reduce input level of brightness
            if(*(imageData + i * lineSize + j) + level > colorScope) {
                //if the result is greater than the maxinium value of color, it equals the maxinium value.
                *(imageData + i * lineSize + j) = colorScope;
            }
            else if(*(imageData + i * lineSize + j) + level < 0) {
                //if the result is less than 0, it equals 0.
                *(imageData + i * lineSize + j) = 0;
            }
            else {
                *(imageData + i * lineSize + j) += level;
            }
        }
    }
}

/*******************************************************************************
*   this is an optional extension made for the part4 of this assignment        *
*   this function changes image to a black-and-white image, it assign the      *
*   average value of 3 color bytes to 3 color bytes in a pixel
*
*   A SMALL FUNCTION MADE JUST FOR FUN!
*   the command argument option of this function is '-b'        *
*
*   the flag of this function is at position 5 (1 << 5);        *
*******************************************************************************/
void monochromeExtension() {
    // switching check
    char checkFlag = 0b00100000; // position 5
    if((checkFlag & flag) == 0) return;

    int i, j, average;
    for(i = 0; i < infoHeader.height; i ++) {
        for(j = 0; j < lineSize - 3; j += 3) {
            // calculating average value
            average = (*(imageData + i * lineSize + j) +\
                       *(imageData + i * lineSize + j + 1) +\
                       *(imageData + i * lineSize + j + 2)) / 3;
            // get monochrome
            *(imageData + i * lineSize + j)     = average;
            *(imageData + i * lineSize + j + 1) = average;
            *(imageData + i * lineSize + j + 2) = average;

        }
    }
}

/*  main  */
int main(int argc, char *argv[]) {
    char *inputFileName;
    FILE *inputFile;
    // for part2 - thresholdFilter coefficient (0.0-1.0)
    double thresholdValue;
    // the input arguments of cropping image (0.0-1.0)
    double newWidthStartProp, newWidthEndProp, newHeightStartProp, newHeightEndProp;
    // the coefficient of setBrightnessExtension function (-1.0-1.0)
    double coefBrightness;

    //looping through command line arguments
    if(argc <= 1) {
        fprintf(stderr, "*Input Error: Command-line parameters needed\n");
        fprintf(stderr, "*Please check help document (-h).\n");
        exit(1);
    }
    int index;
    for(index = 1; index < argc; index ++) { // argument index start from 1
        if(argv[index][0] == '-') {
            switch(argv[index][1]) {
                case 'o': // set the name of the output file, flag 0
                    // case: save by using default name
                    if(argc <= (index + 1) || argv[index + 1][0] == '-') {
                        flag = flag | (1 << 0);
                        break;
                    }
                    // set name
                    outputName = argv[index + 1];
                    // set flag
                    flag = flag | (1 << 0);
                    break;

                case 't': // set the threshold value, flag 1
                    thresholdValue = (double)atof(argv[index + 1]);
                    //set flag
                    flag = flag | (1 << 1);
                    break;

                case 'h': // display usage message, flag 2
                    //set flag
                    flag = flag | (1 << 2);
                    break;

                case 'c': // cropping image, flag 3
                    // set flag
                    flag = flag | (1 << 3);
                    // assign new width, new height of image
                    int i, j;
                    for(i = 0; i < strlen(argv[index + 1]); i ++) {
                        // dividing input into two parts first, then dividing
                        // these two parts separately into 4 parts.
                        if(argv[index + 1][i] == ':') {
                            argv[index + 1][i] = '\0';
                            // dividing argument
                            char newWidthCombined[strlen(argv[index + 1])];
                            strcpy(newWidthCombined, argv[index + 1]);
                            char newHeightCombined[strlen(argv[index + 1])];
                            strcpy(newHeightCombined, argv[index + 1] + i + 1);
                            // dividing arguments
                            for(j = 0; j < strlen(newWidthCombined); j ++) {
                                if(newWidthCombined[j] == ',') {
                                    newWidthCombined[j] = '\0';
                                    newWidthStartProp = atof(newWidthCombined);
                                    newWidthEndProp = atof(newWidthCombined + j + 1);
                                }
                            }
                            for(j = 0; j < strlen(newHeightCombined); j ++) {
                                if(newHeightCombined[j] == ',') {
                                    newHeightCombined[j] = '\0';
                                    newHeightStartProp = atof(newHeightCombined);
                                    newHeightEndProp = atof(newHeightCombined + j + 1);
                                }
                            }
                            break;
                        }
                    }

                    case 'l': // set brightness level, flag 4
                        coefBrightness = (double)atof(argv[index + 1]);
                        //set flag
                        flag = flag | (1 << 4);
                        break;

                    case 'b': // monochrome, flag 5
                        // set flag
                        flag = flag | (1 << 5);
                        break;

                    default:
                        break;

            }
        }
        else if(argv[index - 1][0] != '-') {
            inputFileName = argv[index];
        }
        // -h option been chosen
        else if(argv[index - 1][0] == '-' && \
                (argv[index - 1][1] == 'h' || argv[index - 1][1] == 'b')) {
            inputFileName = argv[index];
        }
    }

    // show usage message
    displayUsageMessage();

    // error check, assign file handler
    inputFile = fopen(inputFileName, "rb+");
    if(inputFile == NULL) { // file check
        fprintf(stderr, "*File Error: The target file is invalid.\n");
        fprintf(stderr, "*Please check help document (-h).\n");
        fprintf(stderr, "*Your Input: %s\n", inputFileName);
        exit(1);
    } else {
        flag = flag | (1 << 7); // set
    }

    // Reading image data in
    readInfo(inputFile);

    // Assignment part2
    thresholdFilter(thresholdValue);

    // optional extesion1
    cropImageExtension(newWidthStartProp, newWidthEndProp, newHeightStartProp, newHeightEndProp);

    // set brightness
    setBrightnessExtension(coefBrightness);

    // monochrome
    monochromeExtension();

    // save image
    saveImage(inputFile);

    free(imageData);
    fclose(inputFile);
    return 0;
}
