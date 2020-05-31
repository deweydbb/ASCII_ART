#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cell.h"

// determines the number of rows and columns of the
// brightness array of fonts and characters
const int NUM_BRIGHT_ROW = 4;
const int NUM_BRIGHT_COL = 2;
// SEC_LEN specifies the length of each section.
// For example if SEC_LEN is 2, the section will be made up of a square of 4 pixels (since 2
// squared is 4). This mean that each cell of the image that is 4 pixels wide and 8 pixels wide
// will be converted to one character.
const int SEC_LEN = 1;

char *IMG = "../images/testGif5.gif";
char *FONT_FILE = "../fontInfo.txt";
char *TEXT_OUTPUT = "../result.txt";
char *GIF_OUTPUT = "../output.gif";
char *IMG_OUTPUT = "../results.jpg";

// given a file path and mode, opens up a file
// and returns a pointer to the file
// exits program if fails to load file
FILE *openFile(char *path, char *mode) {
    FILE *f = fopen(path, mode);

    if (f == NULL) {
        printf("failed to load file: %s\n", path);
        exit(1);
    }

    return f;
}

// takes in an image and converts it to ascii art. Writes output to both
// TEXT_OUTPUT and IMG_OUTPUT. (if gif writes img to ../imageOutput/frames_0x.jpg
//_0x is based on the frame num.
void handleImage(Character *chars, Font font, Image *image, int frameNum) {
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
            fprintf(TEXT_FILE, "%c", bestChar.symbol);
            resultChars[resultCharIndex] = bestChar.symbol;
            resultCharIndex++;
            // free up cell because we no longer need any of its values
            free(c.val_array);
            free(c.bright_array);
        }

        fprintf(TEXT_FILE, "\n");
    }

    fclose(TEXT_FILE);
    free(image->pix);
    free(image);
    // write jpg of ascii art
    createJpgOfResult(resultChars, chars, font, numCellsPerRow, numCellsPerCol, frameNum);
}


int main() {
    FILE *fontInfo = openFile(FONT_FILE, "r");
    Font font = loadFont(fontInfo);
    // stores all characters of the font, and their pixel representations
    Character *chars = getCharArray(font, fontInfo);
    fclose(fontInfo);

    // determine if input file is a GIF or a regular photo
    if (strstr(IMG, ".gif")) {
        // loads gifIn into memory
        Gif *gifIn = getGif(IMG);
        // write out each frame of gifIn to ../imageOutput/frame_0x.jpg
        // then each jpg will be converted to ascii art
        writeGif(gifIn);

        // loop through each frame in the gifIn and convert it to ascii art
        for (int frameNum = 0; frameNum < gifIn->numFrames; frameNum++) {
            char ch[30];
            sprintf(ch, "../imageOutput/frame_0%d.jpg", frameNum);

            Image *image = getImage(ch);
            handleImage(chars, font, image, frameNum);
        }

        // the width and height stored in gifIn in not necessarily the same as
        // the width and height of the ascii images, so gifOut must be initialized
        // after the first frame is loaded in.
        ge_GIF *gifOut = NULL;

        // loop through all frames in gif that have been converted to ascii art
        for (int frameNum = 0; frameNum < gifIn->numFrames; frameNum++) {
            char ch[30];
            sprintf(ch, "../imageOutput/frame_0%d.jpg", frameNum);
            Image *image = getImage(ch);
            // setup gifOut properties
            if (gifOut == NULL) {
                gifOut = getGifOut(image);
            }

            // loop through pixels in image. Because ascii art is either black or white
            // the 2 colors can be represented by 1 bit (0 or 1). 0 represents black
            // 1 represents white
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

            // free up image memory as it is no longer needed
            free(image->pix);
            free(image);
            // add frame to gifOut with appropriate delay
            ge_add_frame(gifOut, gifIn->delay / gifIn->numFrames);
        }

        // free up gifIn
        free(gifIn);
        // write gif to file and clean up
        ge_close_gif(gifOut);
    } else {
        // input is an image
        Image *image = getImage(IMG);
        // -1 for frame number signals that this image is not part of gif
        // and the regular IMG_OUTPUT path should be used
        handleImage(chars, font, image, -1);
    }

    return 0;
}


