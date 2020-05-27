#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Cell.h"

Image *getImage(char *path) {
    int width, height, channels;
    unsigned char *pixels = stbi_load(path, &width, &height, &channels, 1);

    if (pixels == NULL) {
        printf("Error in loading image\n");
        exit(1);
    }

    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->pix = pixels;

    return img;
}

short getPixel(Image *image, int startRow, int startCol, int ratio) {
    int sum = 0;

    for (int i = 0; i < ratio; i++) {
        for (int j = 0; j < ratio; j++) {
            int index = (startRow + j) * image->width + (startCol + i);
            sum += image->pix[index];
        }
    }

    return (short) (sum / ratio);
}

Cell getCell(Image *image, int cellRow, int cellCol, Font font, int ratio) {
    int startRow = cellRow * font.height * ratio;
    int startCol = cellCol * font.width * ratio;

    Cell c;
    c.width = font.width;
    c.height = font.height;
    c.minVal = 255;

    c.val_array = malloc(c.width * c.height * sizeof(short));

    for (int row = 0; row < c.height; row++) {
        for (int col = 0; col < c.width; col++) {
            int index = row * c.width + col;

            short pix = getPixel(image, startRow + (row * ratio), startCol + (col * ratio), ratio);
            if (pix < c.minVal) {
                c.minVal = pix;
            }

            c.val_array[index] = pix;
        }
    }

    return c;
}



// returns a number representing the difference between a cell and a character
// 0 means they are identical, the larger the number the less similar they are
// pre: cell.width = font.width, cell.height = font.height
int compareCellToChar(Cell cell, Character ch, Font font) {
    if (cell.width != font.width || cell.height != font.height) {
        printf("cell and font dimensions do not match");
        exit(1);
    }

    int totalDiff = 0;

    for (int row = 0; row < cell.height; row++) {
        for (int col = 0; col < cell.width; col++) {
            int index = row * cell.width + col;

            //int cellVal = ch.val_array[index] == 255 ? 255 : cell.minVal;

            totalDiff += abs(cell.val_array[index] - ch.val_array[index]);
        }
    }

    return totalDiff;
}

Character getBestChar(Cell cell, Character *chars, Font font) {
    int bestIndex = 0;
    int lowestDiff = INT_MAX;

    for (int i = 0; i < font.numChar; i++) {
        int diff = compareCellToChar(cell, chars[i], font);
        if (diff < lowestDiff) {
            lowestDiff = diff;
            bestIndex = i;
        }
    }

    return chars[bestIndex];
}



