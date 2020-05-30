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

Cell getCell(Image *image, int row, int col, int secLen);

Character getBestChar(Cell cell, Character *chars, Font font);

void printCellBrightness(Cell cell);

#endif //ASCII_ART_CELL_H
