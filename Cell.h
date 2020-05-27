#include "font.h"

#ifndef ASCII_ART_CELL_H
#define ASCII_ART_CELL_H

typedef struct {
    int width;
    int height;
    int minVal;
    short *val_array;
} Cell;

typedef struct {
    int width;
    int height;
    unsigned char *pix
} Image;

Image *getImage(char *path);

Cell getCell(Image *image, int row, int col, Font font, int ratio);

int compareCellToChar(Cell cell, Character ch, Font font);

Character getBestChar(Cell cell, Character *chars, Font font);

#endif //ASCII_ART_CELL_H
