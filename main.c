#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cell.h"
#include "File.h"

// determines the number of rows and columns of the
// brightness array of fonts and characters
const int NUM_BRIGHT_ROW = 4;
const int NUM_BRIGHT_COL = 2;
// SEC_LEN specifies the length of each section.
// For example if SEC_LEN is 2, the section will be made up of a square of 4 pixels (since 2
// squared is 4). This mean that each cell of the image that is 4 pixels wide and 8 pixels wide
// will be converted to one character.
const int SEC_LEN = 1;

char *IMG;
char FONT_FILE[16] = "../fontInfo.txt";
char *TEXT_OUTPUT = NULL;
char *GIF_OUTPUT = NULL;
char *IMG_OUTPUT = NULL;

// takes in an image and converts it to ascii art. Writes output to both
// TEXT_OUTPUT and IMG_OUTPUT. (if gif writes img to ../imageOutput/frames_0x.jpg
//_0x is based on the frame num.
Image *handleImage(Character *chars, Font font, Image *image) {
    printf("image width: %d, height: %d\n", image->width, image->height);

    FILE *TEXT_FILE = openFile(TEXT_OUTPUT, "w");

    int numCellsPerRow = image->height / (SEC_LEN * NUM_BRIGHT_ROW);
    int numCellsPerCol = image->width / (SEC_LEN * NUM_BRIGHT_COL);

    // keeps track of order of characters to allow for faster creation of the image output
    unsigned char *resultChars = malloc(numCellsPerRow * numCellsPerCol * sizeof(char));
    int resultCharIndex = 0;

    // loop through each cell of image, determine best character
    // and write that character to a file
    for (int row = 0; row < numCellsPerRow; row++) {
        for (int col = 0; col < numCellsPerCol; col++) {
            Cell c = getCell(image, row, col, SEC_LEN);
            // get best char to represent current cell
            Character bestChar = getBestChar(c, chars, font);
            // write best char to text file
            if (TEXT_FILE != NULL) {
                fprintf(TEXT_FILE, "%c", bestChar.symbol);
            }
            resultChars[resultCharIndex] = bestChar.symbol;
            resultCharIndex++;
            // free up cell because we no longer need any of its values
            free(c.val_array);
            free(c.bright_array);
        }

        if (TEXT_FILE != NULL) {
            fprintf(TEXT_FILE, "\n");
        }
    }
    if (TEXT_FILE != NULL) {
        fclose(TEXT_FILE);
    }

    return createPixelResult(resultChars, chars, font, numCellsPerRow, numCellsPerCol);
}

void handleGif(Gif *gifIn, Character *chars, Font font) {
    // stores ascii version of frames for gif
    Image *imgPointer[gifIn->numFrames];

    // loop through each frame from input gif and covert it to ascii art
    for (int frameNum = 0; frameNum < gifIn->numFrames; frameNum++) {
        Image *inputImg = malloc(sizeof(Image));
        inputImg->width = gifIn->width;
        inputImg->height = gifIn->height;
        inputImg->pix = gifIn->pix + gifIn->width * gifIn->height * frameNum;

        imgPointer[frameNum] = handleImage(chars, font, inputImg);
    }

    free(gifIn->pix);

    ge_GIF *gifOut = getGifOut(imgPointer[0]);

    printf("output width: %d output height %d\n", imgPointer[0]->width, imgPointer[0]->height);

    for (int frameNum = 0; frameNum < gifIn->numFrames; frameNum++) {
        Image *image = imgPointer[frameNum];

        // loop through every pixel in frame
        for (int row = 0; row < image->height; row++) {
            for (int col = 0; col < image->width; col++) {
                int index = row * image->width + col;
                // get value of pixel in ascii art. Should always be 0 or 255
                int val = image->pix[index];

                if (val == 0) {
                    // mark pixel in gifOut frame as black
                    gifOut->frame[index] = 0;
                } else {
                    // mark pixel in gifOut frame as white
                    gifOut->frame[index] = 1;
                }
            }
        }
        // add frame to gifOut with appropriate delay
        ge_add_frame(gifOut, gifIn->delay / gifIn->numFrames);
    }


    // free up gifIn
    free(gifIn);
    // write gif to file and clean up
    ge_close_gif(gifOut);
}

int main() {
    setInputAndOutputPath();
    char *imgPath = IMG;

    FILE *fontInfo = openFile(FONT_FILE, "r");
    Font font = loadFont(fontInfo);
    // stores all characters of the font, and their pixel representations
    Character *chars = getCharArray(font, fontInfo);
    fclose(fontInfo);

    // determine if input file is a GIF or a regular photo
    if (strstr(imgPath, ".gif")) {
        // loads gifIn into memory
        Gif *gifIn = getGif(IMG);
        // converts each frame to image and then combines each frame into gif
        handleGif(gifIn, chars, font);
    } else {
        // input is an image
        Image *image = getImage(imgPath);
        // -1 for frame number signals that this image is not part of gif
        // and the regular IMG_OUTPUT path should be used
        Image *output = handleImage(chars, font, image);
        // write to file and free up image
        if (IMG_OUTPUT != NULL) {
            createJpgOfResult(output);
        }
    }

    return 0;
}


