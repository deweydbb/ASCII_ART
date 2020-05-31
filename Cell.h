#include "Image.h"
#include <stdlib.h>

#ifndef ASCII_ART_CELL_H
#define ASCII_ART_CELL_H

typedef struct {
    int width;
    int height;
    short *val_array;
    double *bright_array;
} Cell;

// given an image and the row and column of the cell
// returns the cell at the given location in the image
Cell getCell(Image *image, int row, int col, int secLen);

// given a cell and the array of characters of a font
// returns the character that is the best match for the given cell
Character getBestChar(Cell cell, Character *chars, Font font);

// prints the brightness array of a cell. used for debugging purposes
void printCellBrightness(Cell cell);

#endif //ASCII_ART_CELL_H
