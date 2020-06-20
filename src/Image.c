#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"
#include <stdio.h>

extern char *IMG_OUTPUT;
extern char *GIF_OUTPUT;

/*
 * Both gif and image functions need to be in this file because of the image library I am using
 * The image/gif library (stb_image.h and stb_image_write.h) can only be included in one file
 * in a project which forces both the image and gif code to be in this file
 */

// returns a struct that contains an array of pixels, width and height of an image
// in grey scale. Exits program if unable to load image.
Image *getImage(char *path) {
    int width, height, channels;
    unsigned char *pixels = stbi_load(path, &width, &height, &channels, 1);

    // failed to load image, exit program
    if (pixels == NULL) {
        printf("Failed to load image: %s\n", path);
        exit(1);
    }
    // create image struct
    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->pix = pixels;

    return img;
}

// helper method for createJpgOfResult
// given the order of characters as they appear in the ascii art, creates an image that holds the info necessary to
// write the image to a jpg file
Image *createPixelResult(const unsigned char *charResult, Character *chars, Font font, int charPerRow, int charPerCol) {
    Image *result = malloc(sizeof(Image));
    // set width and height of image
    result->width = font.width * charPerCol;
    result->height = font.height * charPerRow;

    result->pix = malloc(result->width * result->height * sizeof(char));

    int pixelIndex = 0;
    // loop through all the rows of characters
    for (int row = 0; row < charPerRow; row++) {
        // loops through all the pixel rows in a character
        for (int fontRow = 0; fontRow < font.height; fontRow++) {
            // loop through all the columns of characters
            for (int col = 0; col < charPerCol; col++) {
                // get the character at the given character row and column
                unsigned char currentChar = charResult[row * charPerCol + col];
                Character *ch = getCharacterFromSymbol(chars, font, currentChar);
                // loop through all the pixel columns in a character
                for (int fontCol = 0; fontCol < font.width; fontCol++) {
                    // get the value of the pixel at the current location in the image
                    int val_arrayIndex = fontRow * font.width + fontCol;
                    unsigned char val = ch->val_array[val_arrayIndex];
                    // add pixel value to image array of pixels
                    result->pix[pixelIndex] = val;
                    pixelIndex++;
                }
            }
        }
    }
    // return image
    return result;
}

// given the order of characters in the ascii art, writes a jpg of the ascii art to IMG_OUTPUT
void createJpgOfResult(Image *img) {
    // write image to specified file path. comp of 1 means image is black and white
    stbi_write_jpg(IMG_OUTPUT, img->width, img->height, 1, img->pix, img->width);

    // free up image
    free(img->pix);
    free(img);
}

/*
 * ----------------------
 * START OF GIF FUNCTIONS
 * ----------------------
 */

// loads in gif from file
Gif *getGif(const char *filename) {
    FILE *gif = fopen(filename, "rb");
    if (gif == NULL) {
        printf("failed to load gif: %s\n", filename);
        exit(1);
    }

    // code for correctly loading in gif adapted from:
    // https://github.com/nothings/stb/issues/915#issuecomment-604217649
    // determine size of gif
    fseek(gif, 0L, SEEK_END);
    int size = (int) ftell(gif);
    void *buffer = malloc(size);
    fseek(gif, 0L, SEEK_SET);
    // read gif into buffer
    fread(buffer, size, 1, gif);
    fclose(gif);

    // variables to be set by load_gif_from_memory
    int *delays = NULL;
    int width, height, frames, comp;
    int req_comp = 1;

    stbi_uc *loadedGif = stbi_load_gif_from_memory(buffer, size, &delays, &width, &height, &frames, &comp, req_comp);
    free(buffer);

    if (loadedGif == NULL) {
        printf("failed to load gif: %s\n", filename);
        exit(1);
    }

    // setup gif struct
    Gif *result = malloc(sizeof(Gif));
    result->width = width;
    result->height = height;
    result->numFrames = frames;
    result->delay = *delays;
    result->pix = loadedGif;

    return result;
}

// given an image initializes a ge_GIF struct
// to the correct width, height, file path, color palette etc
ge_GIF *getGifOut(Image *image) {
    ge_GIF *gifOut = ge_new_gif(
            GIF_OUTPUT,                 // name of gif
            image->width,               // width of canvas
            image->height,              // height of canvas
            (uint8_t[]) {               // palette
                    0x00, 0x00, 0x00,   // 0 -> black
                    0xFF, 0xFF, 0xFF    // 1 -> white
            },
            1,                          // palette depth == log2(# of colors)
            0                            // infinite loop
    );

    return gifOut;
}
