//
// Created by dewey on 6/16/2020.
//
#include <stdlib.h>
#include "tinyfiledialogs.h"
#include "File.h"

extern char *IMG;
extern char *TEXT_OUTPUT;
extern char *GIF_OUTPUT;
extern char *IMG_OUTPUT;

// given a file path and mode, opens up a file
// and returns a pointer to the file
// exits program if fails to load file
FILE *openFile(char *path, char *mode) {
    if (path == NULL) {
        return NULL;
    }

    FILE *f = fopen(path, mode);

    if (f == NULL) {
        printf("failed to load file: %s\n", path);
        exit(1);
    }

    return f;
}

void setInputPath() {
    char const *filterPatterns[8] = {"*.jpg", "*.png", "*.gif", "*.tga", "*.bmp", "*.psd", "*.hdr", "*.pic"};
    char *inputPath = NULL;

    while (inputPath == NULL) {
        inputPath = tinyfd_openFileDialog(
                "Please pick an image to convert",
                "",
                8,
                filterPatterns,
                NULL,
                0);

        if (inputPath == NULL) {
            tinyfd_messageBox("Error", "Please select an image or gif to convert into ascii art",
                    "ok", "info", 0);
        }
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

void setOutputPath(int isInputGif, int isOutputText) {
    char *filterPatterns[2] = {"*.txt", "*.text"};
    int numPatters = 2;
    if (isInputGif) {
        numPatters = 1;
        filterPatterns[0] = "*.gif";
    } else if (!isOutputText) {
        numPatters = 1;
        filterPatterns[0] = "*.jpg";
    }

    char *outputPath = NULL;
    while (outputPath == NULL) {
        outputPath = tinyfd_saveFileDialog(
                "Choose where the ascii art will be saved",
                "output",
                numPatters,
                (const char *const *) filterPatterns,
                NULL);

        if (outputPath == NULL) {
            tinyfd_messageBox("Error", "Please select where to save the ascii art",
                              "ok", "info", 0);
        }
    }

    if (isOutputText) {
        TEXT_OUTPUT = strdup(outputPath);
    } else if (isInputGif) {
        GIF_OUTPUT = strdup(outputPath);
    } else {
        IMG_OUTPUT = strdup(outputPath);
    }
}

void setInputAndOutputPath() {
    setInputPath();

    int isInputGif = isGif(IMG);
    int isOutputText = 0;
    if (!isInputGif) {
        isOutputText = tinyfd_messageBox(
                "Question",
                "Do you want the output to be a text file?",
                "yesno",
                "question",
                1);
    }

    setOutputPath(isInputGif, isOutputText);
}


