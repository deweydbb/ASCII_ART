#include "font.h"
#include "../libs/gifenc.h"

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

Image *createPixelResult(const unsigned char *charResult, Character *chars, Font font, int charPerRow, int charPerCol);

// given the order of characters in the ascii art, writes a jpg of the ascii art to IMG_OUTPUT
void createJpgOfResult(Image *image);

typedef struct {
    int width;
    int height;
    int numFrames;
    int delay;
    unsigned char *pix;
} Gif;

// loads in gif from file
Gif *getGif(const char *filename);

// given an image initializes a ge_GIF struct
// to the correct width, height, file path, color palette etc
ge_GIF *getGifOut(Image *image);

#endif //ASCII_ART_IMAGE_H
