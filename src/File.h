#include <stdio.h>
#include <string.h>

#ifndef ASCII_ART_FILE_H
#define ASCII_ART_FILE_H

// given a file path and mode, opens up a file
// and returns a pointer to the file
// exits program if fails to load file
FILE *openFile(char *path, char *mode);

void resetPaths();

// given a title and a message, sends a
// a popup window with given info
void sendPopup(char *title, char *msg);

int getSecLenFromUser();

// get input and output paths from user
void setInputAndOutputPath();

#endif //ASCII_ART_FILE_H
