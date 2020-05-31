#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cell.h"

const int NUM_BRIGHT_ROW = 4;
const int NUM_BRIGHT_COL = 2;
const int SEC_LEN = 1;

char *IMG = "D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/images/testGif5.gif";
char *FONT_FILE = "D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/fontInfo.txt";
char *TEXT_OUTPUT = "D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/result.txt";
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

void handleImage(Character *chars, Font font, Image *image, int fileNum) {
    printf("image width: %d, height: %d\n", image->width, image->height);

    FILE *result = openFile(TEXT_OUTPUT, "w");

    int numCellsPerRow = image->height / (SEC_LEN * NUM_BRIGHT_ROW);
    int numCellsPerCol = image->width / (SEC_LEN * NUM_BRIGHT_COL);

    unsigned char *resultChars = malloc(numCellsPerRow * numCellsPerCol * sizeof(char));
    int resultCharIndex = 0;

    // loop through each cell of image, determine best character
    // and write that character to a file
    for (int row = 0; row < numCellsPerRow; row++) {
        for (int col = 0; col < numCellsPerCol; col++) {
            Cell c = getCell(image, row, col, SEC_LEN);

            Character bestChar = getBestChar(c, chars, font);

            fprintf(result, "%c", bestChar.symbol);
            resultChars[resultCharIndex] = bestChar.symbol;
            resultCharIndex++;
            // free up cell because we no longer need any of its
            // values
            free(c.val_array);
            free(c.bright_array);
        }

        fprintf(result, "\n");
    }

    fclose(result);
    freeImage(image);

    createJpgOfResult(resultChars, chars, font, numCellsPerRow, numCellsPerCol, fileNum);
}


int main() {
    FILE *fontInfo = openFile(FONT_FILE, "r");
    Font font = loadFont(fontInfo);
    Character *chars = getCharArray(font, fontInfo);
    fclose(fontInfo);

    if (strstr(IMG, ".gif")) {
        Gif *gif = getGif(IMG);
        writeGif(gif);

        for (int i = 0; i < gif->numFrames; i++) {
            char ch[80];
            sprintf(ch, "D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/imageOutput/frame_0%d.jpg", i);

            Image *image = getImage(ch);
            handleImage(chars, font, image, i);
        }

        ge_GIF *gifOut = NULL;

        for (int i = 0; i < gif->numFrames; i++) {
            char ch[80];
            sprintf(ch, "D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/imageOutput/frame_0%d.jpg", i);
            Image *image = getImage(ch);

            if (gifOut == NULL) {
                gifOut = getGifOut(image);
            }


            for (int row = 0; row < image->height; row++) {
                for (int col = 0; col < image->width; col++) {
                    int index = row * image->width + col;
                    int val = image->pix[index];

                    if (val == 0) {
                        gifOut->frame[index] = 0;
                    } else {
                        gifOut->frame[index] = 1;
                    }
                }
            }


            free(image->pix);
            free(image);

            ge_add_frame(gifOut, gif->delay / gif->numFrames);
        }

        free(gif);
        ge_close_gif(gifOut);
    } else {
        Image *image = getImage(IMG);
        handleImage(chars, font, image, -1);
    }

    return 0;
}


