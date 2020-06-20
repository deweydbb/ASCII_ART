//
// Created by dewey on 6/16/2020.
//
#include <stdlib.h>
#include "../libs/tinyfiledialogs.h"
#include "File.h"

extern char *IMG;
extern char *TEXT_OUTPUT;
extern char *GIF_OUTPUT;
extern char *IMG_OUTPUT;

// given a file path and mode, opens up a file
// and returns a pointer to the file
// if path is null returns null
// exits program if fails to load file with non null path
FILE *openFile(char *path, char *mode) {
    if (path == NULL) {
        return NULL;
    }

    FILE *f = fopen(path, mode);

    if (f == NULL) {
        printf("failed to load file: %s\n", path);
        char *msg = NULL;
        sprintf(msg, "failed to load file: %s", path);
        sendPopup("Error", msg);
        exit(1);
    }

    return f;
}

// given a title and a message, sends a
// a popup window with given info
void sendPopup(char *title, char *msg) {
    tinyfd_messageBox(title, msg,"ok", "info", 0);
}

// set the global variable IMG which stores the path of the input image
// uses gui to have user select file they want to convert
void setInputPath() {
    char const *filterPatterns[8] = {"*.jpg", "*.png", "*.gif", "*.tga",
                                     "*.bmp", "*.psd", "*.hdr", "*.pic"};

    char *inputPath = tinyfd_openFileDialog(
            "Please pick an image to convert",
            "",
            8,
            filterPatterns,
            NULL,
            0);

    if (inputPath == NULL) {
        tinyfd_messageBox("Error", "You did not select an image. Exiting program.",
                          "ok", "info", 0);
        exit(0);
    }


    IMG = strdup(inputPath);
}

int isGif(char *path) {
    unsigned int len = strlen(path);
    const char *GIF = "gif";

    for (int i = 0; i < 3; i++) {
        if (path[len - i] != GIF[3 - i]) {
            return 0;
        }
    }

    return 1;
}

// sets appropriate global variable for output path
// asks user where they want to save the output
void setOutputPath(int isInputGif, int isOutputText) {
    // setup filter patterns based on input file type
    char *filterPatterns[2] = {"*.txt", "*.text"};
    int numPatters = 2;
    if (isInputGif) {
        numPatters = 1;
        filterPatterns[0] = "*.gif";
    } else if (!isOutputText) {
        numPatters = 1;
        filterPatterns[0] = "*.jpg";
    }

    char *outputPath = tinyfd_saveFileDialog(
            "Choose where the ascii art will be saved",
            "output",
            numPatters,
            (const char *const *) filterPatterns,
            NULL);

    if (outputPath == NULL) {
        tinyfd_messageBox("Error", "You did not select a file. Exiting program.",
                          "ok", "info", 0);
    }
    // set correct global variable based on input file type
    if (isOutputText) {
        TEXT_OUTPUT = strdup(outputPath);
    } else if (isInputGif) {
        GIF_OUTPUT = strdup(outputPath);
    } else {
        IMG_OUTPUT = strdup(outputPath);
    }
}

// get input and output paths from user
void setInputAndOutputPath() {
    setInputPath();

    int isInputGif = isGif(IMG);
    int isOutputText = 0;
    // if the user wants to convert and image determine
    // if they want the output to be a .txt file or a jpg
    if (!isInputGif) {
        isOutputText = tinyfd_messageBox(
                "Question",
                "Do you want the output to be a .txt file? (No for .jpg)",
                "yesno",
                "question",
                1);
    }

    setOutputPath(isInputGif, isOutputText);
}


