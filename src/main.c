#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
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
int SEC_LEN = 1;

char *IMG = NULL;
char *TEXT_OUTPUT = NULL;
char *GIF_OUTPUT = NULL;
char *IMG_OUTPUT = NULL;

pthread_mutex_t statusLock;
char status[1024] = "converting";
int inputIsGif = 0;

// takes in an image and converts it to ascii art. Writes output to both
// TEXT_OUTPUT and IMG_OUTPUT. (if gif writes img to ../imageOutput/frames_0x.jpg
//_0x is based on the frame num.
Image *handleImage(Character *chars, Font font, Image *image) {
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
            if (!inputIsGif) {
                pthread_mutex_lock(&statusLock);
                sprintf(status, "analyzing cell %d of %d",
                        row * numCellsPerCol + col, numCellsPerCol * numCellsPerRow);
                pthread_mutex_unlock(&statusLock);
            }

            Cell c = getCell(image, row, col, SEC_LEN);
            // get best char to represent current cell
            Character bestChar = getBestChar(c, chars, font);
            if (TEXT_FILE != NULL) {
                // write best char to text file
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

    if (!inputIsGif) {
        pthread_mutex_lock(&statusLock);
        sprintf(status, "creating image");
        pthread_mutex_unlock(&statusLock);
    }
    return createPixelResult(resultChars, chars, font, numCellsPerRow, numCellsPerCol);
}

void *handleFrames(void *gifInfo) {
    GifThreadInfo *gifThreadInfo = gifInfo;

    for (int frameNum = gifThreadInfo->startFrame; frameNum < gifThreadInfo->endFrame; frameNum++) {
        Image *inputImg = malloc(sizeof(Image));
        inputImg->width = gifThreadInfo->gifIn->width;
        inputImg->height = gifThreadInfo->gifIn->height;
        inputImg->pix = gifThreadInfo->gifIn->pix + gifThreadInfo->gifIn->width * gifThreadInfo->gifIn->height * frameNum;

        gifThreadInfo->asciiImages[frameNum] = handleImage(gifThreadInfo->chars, *gifThreadInfo->font, inputImg);
    }
}

// converts a gif to separate ascii art images and then rejoins images as
// frames to create a gif of ascii art
void handleGif(Gif *gifIn, Character *chars, Font font) {
    // stores ascii version of frames for gif
    Image **imgPointer = malloc(gifIn->numFrames * sizeof(Image));
    int numThreads = 8;

    GifThreadInfo gifInfo[numThreads];
    pthread_t pIds[numThreads];

    for (int i = 0; i < numThreads; i++) {
        gifInfo[i].gifIn = gifIn;
        gifInfo[i].chars = chars;
        gifInfo[i].font = &font;
        gifInfo[i].asciiImages = imgPointer;
        gifInfo[i].startFrame = i * gifIn->numFrames / numThreads;
        gifInfo[i].endFrame = (i + 1) * gifIn->numFrames / numThreads;

        int error = pthread_create(&(pIds[i]), NULL, &handleFrames, &gifInfo[i]);
        if (error != 0) {
            printf("\nThread can't be created :[%s]", strerror(error));
        }
    }

    for(int i = 0; i < numThreads; i++) {
        pthread_join(pIds[i], NULL);
    }

    free(gifIn->pix);

    ge_GIF *gifOut = getGifOut(imgPointer[0]);

    for (int frameNum = 0; frameNum < gifIn->numFrames; frameNum++) {
        Image *image = imgPointer[frameNum];
        pthread_mutex_lock(&statusLock);
        sprintf(status, "saving frame %d of %d", frameNum, gifIn->numFrames);
        pthread_mutex_unlock(&statusLock);

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


    free(imgPointer);
    // free up gifIn
    free(gifIn);
    // write gif to file and clean up
    ge_close_gif(gifOut);
}

void *progressThread(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        pthread_mutex_lock(&statusLock);
        int oldStatusLen = strlen(status);
        printf(status);
        pthread_mutex_unlock(&statusLock);
        sleep(1);
        printf(".");
        sleep(1);
        printf(".");
        sleep(1);
        printf(".");
        sleep(1);
        fflush(stdout);
        for (int i = 0; i < oldStatusLen + 3; i++) {
            printf("\b");
        }

        for (int i = 0; i < oldStatusLen + 3; i++) {
            printf(" ");
        }

        for (int i = 0; i < oldStatusLen + 3; i++) {
            printf("\b");
        }

        fflush(stdout);
    }
}

int main() {
    // get input and output paths from user
    setInputAndOutputPath();
    SEC_LEN = getSecLenFromUser();

    Font font = loadFont();
    // stores all characters of the font, and their pixel representations
    Character *chars = getCharArray(font);

    clock_t start = clock();

    pthread_t threadId;
    int error = pthread_create(&threadId, NULL, &progressThread, NULL);
    if (error != 0) {
        printf("\nThread can't be created :[%s]", strerror(error));
    }

    // determine if input file is a GIF or a regular photo
    if (strstr(IMG, ".gif")) {
        inputIsGif = 1;
        // loads gifIn into memory
        Gif *gifIn = getGif(IMG);
        // converts each frame to image and then combines each frame into gif
        handleGif(gifIn, chars, font);
    } else {
        // input is an image
        Image *image = getImage(IMG);
        // -1 for frame number signals that this image is not part of gif
        // and the regular IMG_OUTPUT path should be used
        Image *output = handleImage(chars, font, image);
        if (IMG_OUTPUT != NULL) {
            // create jpg of ascii art
            pthread_mutex_lock(&statusLock);
            sprintf(status, "saving image");
            pthread_mutex_unlock(&statusLock);

            createJpgOfResult(output);
        }
    }

    pthread_cancel(threadId);
    pthread_join(threadId, NULL);

    clock_t stop = clock();

    printf("time: %.4ld", stop - start);

    sendPopup("", "Conversion completed successfully!");

    return 0;
}


