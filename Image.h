#include "font.h"
#include "gifenc.h"

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

// given the order of characters in the ascii art, writes a jpg of the ascii art to IMG_OUTPUT
// if the frameNum is -1. (meaning image is stand alone and not part of gif)
void createJpgOfResult(unsigned char *charResult, Character *chars, Font font, int charPerRow, int charPerCol, int frameNum);

typedef struct {
    int width;
    int height;
    int numFrames;
    int delay;
    unsigned char *pix;
} Gif;

// loads in gif from file
Gif *getGif(const char *filename);

// write all frames of a gif to ../imageOutput/frame_0x.jpg
void writeGif(Gif *gif);

// given an image initializes a ge_GIF struct
// to the correct width, height, file path, color palette etc
ge_GIF *getGifOut(Image *image);

// delete frames created in ../imageOutput/
void cleanUpFrames(Gif *gif);

#endif //ASCII_ART_IMAGE_H
