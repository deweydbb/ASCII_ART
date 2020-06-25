#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
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

//update status message on terminal
void updateStatusMessage(char *msg, int firstNum, int secondNum) {
    pthread_mutex_lock(&statusLock);

    if (firstNum == 0 && secondNum == 0) {
        sprintf(status, "%s", msg);
    } else {
        sprintf(status, "%s %d of %d", msg, firstNum, secondNum);
    }

    pthread_mutex_unlock(&statusLock);
}

// takes in an image and converts it to ascii art. Writes output to both
// TEXT_OUTPUT and IMG_OUTPUT. (if gif writes img to ../imageOutput/frames_0x.jpg
//_0x is based on the frame num.
Image *handleImage(Character *chars, Font font, Image *image) {
    FILE *TEXT_FILE = openFile(TEXT_OUTPUT, "w");

    int numCellsPerRow = image->height / (SEC_LEN * NUM_BRIGHT_ROW);
    int numCellsPerCol = image->width / (SEC_LEN * NUM_BRIGHT_COL);

    // keeps track of order of characters to allow for faster creation of the image output
    char *resultChars = malloc(numCellsPerRow * numCellsPerCol * sizeof(char));
    int resultCharIndex = 0;

    // loop through each cell of image, determine best character
    // and write that character to a file
    for (int row = 0; row < numCellsPerRow; row++) {
        for (int col = 0; col < numCellsPerCol; col++) {
            if (!inputIsGif) {
                updateStatusMessage("analyzing cell", row * numCellsPerCol + col, numCellsPerCol * numCellsPerRow);
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
        updateStatusMessage("creating image", 0, 0);
    }
    // if out put is not a gif, free up the input image
    if (GIF_OUTPUT == NULL) {
        free(image->pix);
        free(image);
    }
    // if output is a text file, then it is not necessary to create a pixel
    // result of the ascii art
    if (TEXT_FILE != NULL) {
        free(resultChars);
        return NULL;
    }

    return createPixelResult(resultChars, chars, font, numCellsPerRow, numCellsPerCol);
}

// function for multi threading gif conversion
// converts given range of frames to ascii art
void *handleFrames(void *gifInfo) {
    GifThreadInfo *gifThreadInfo = gifInfo;

    for (int frameNum = gifThreadInfo->startFrame; frameNum < gifThreadInfo->endFrame; frameNum++) {
        Image *inputImg = malloc(sizeof(Image));
        inputImg->width = gifThreadInfo->gifIn->width;
        inputImg->height = gifThreadInfo->gifIn->height;
        inputImg->pix = gifThreadInfo->gifIn->pix + gifThreadInfo->gifIn->width * gifThreadInfo->gifIn->height * frameNum;

        gifThreadInfo->asciiImages[frameNum] = handleImage(gifThreadInfo->chars, *gifThreadInfo->font, inputImg);
        free(inputImg);
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
    // creates the threads for converting each frame to ascii
    for (int threadNum = 0; threadNum < numThreads; threadNum++) {
        gifInfo[threadNum].gifIn = gifIn;
        gifInfo[threadNum].chars = chars;
        gifInfo[threadNum].font = &font;
        gifInfo[threadNum].asciiImages = imgPointer;
        gifInfo[threadNum].startFrame = threadNum * gifIn->numFrames / numThreads;
        gifInfo[threadNum].endFrame = (threadNum + 1) * gifIn->numFrames / numThreads;

        int error = pthread_create(&(pIds[threadNum]), NULL, &handleFrames, &gifInfo[threadNum]);
        if (error != 0) {
            printf("\nThread can't be created :[%s]", strerror(error));
        }
    }
    // wait for all threads to finish converting their frames
    for(int i = 0; i < numThreads; i++) {
        pthread_join(pIds[i], NULL);
    }

    free(gifIn->pix);

    ge_GIF *gifOut = getGifOut(imgPointer[0]);

    for (int frameNum = 0; frameNum < gifIn->numFrames; frameNum++) {
        updateStatusMessage("saving frame", frameNum, gifIn->numFrames);
        saveGifFrame(imgPointer[frameNum], gifOut);
        // add frame to gifOut with appropriate delay
        ge_add_frame(gifOut, gifIn->delay / gifIn->numFrames);
    }

    free(imgPointer);
    // free up gifIn
    free(gifIn);
    // write gif to file and clean up
    ge_close_gif(gifOut);
}

// displays status message and rotating ellipsis on command line
void *progressThread(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        // get current status
        pthread_mutex_lock(&statusLock);
        int oldStatusLen = strlen(status);
        printf("%s", status);
        pthread_mutex_unlock(&statusLock);
        // start rotating ellipsis
        sleep(1);
        printf(".");
        sleep(1);
        printf(".");
        sleep(1);
        printf(".");
        sleep(1);
        //end ellipsis
        fflush(stdout);
        // move cursor to start of line
        int len = (oldStatusLen + 3) * 3 + 1;
        char delete[len];
        int index = 0;
        while (index < len - 1) {
            if (index < oldStatusLen + 3 || index >= (oldStatusLen + 3) * 2) {
                delete[index] = '\b';
            } else {
                delete[index] = ' ';
            }
            index++;
        }

        delete[len - 1] = 0;

        printf("%s", delete);
        fflush(stdout);
    }
}

int main() {
    Font font = loadFont();
    // stores all characters of the font, and their pixel representations
    Character *chars = getCharArray(font);

    pthread_t threadId;
    int error = pthread_create(&threadId, NULL, &progressThread, NULL);
    if (error != 0) {
        printf("\nProgress thread can't be created :[%s]", strerror(error));
    }

    do {
        // get input and output paths from user
        setInputAndOutputPath();
        SEC_LEN = getSecLenFromUser();

        updateStatusMessage("converting", 0, 0);

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
                updateStatusMessage("saving image", 0, 0);
                // create jpg of ascii art
                createJpgOfResult(output);
            }
        }
        
        updateStatusMessage("", 0, 0);
        sendPopup("", "Conversion completed successfully!");
        // free image paths and set them all to null
        resetPaths();

    } while(continueConverting());

    pthread_cancel(threadId);
    pthread_join(threadId, NULL);

    // free font and characters in font
    for (int i = 0; i < font.numChar; i++) {
        free(chars[i].val_array);
        free(chars[i].bright_array);
    }
    free(chars);

    return 0;
}


