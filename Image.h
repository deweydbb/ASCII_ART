#include "font.h"


#ifndef ASCII_ART_IMAGE_H
#define ASCII_ART_IMAGE_H

// image struct stores width and height of image
// and 1d array of pixels in black and white.
typedef struct {
    int width;
    int height;
    unsigned char *pix;
} Image;

// loads and returns a pointer to the image struct
// based on the given path
Image *getImage(char *path);

// frees up memory associated with an image struct
void freeImage(Image *image);

void createJpgOfResult(unsigned char *charResult, Character *chars, Font font, int charPerRow, int charPerCol, int fileNum);

typedef struct {
    int width;
    int height;
    int numFrames;
    int delay;
    unsigned char *pix;
} Gif;

Gif *getGif(const char *filename);

void writeGif(Gif *gif);

#endif //ASCII_ART_IMAGE_H
