#include <stdio.h>

#ifndef ASCII_ART_FONT_H
#define ASCII_ART_FONT_H

#endif //ASCII_ART_FONT_H

typedef struct {
    int width;
    int height;
    int numChar;
} Font;

typedef struct {
    char symbol;
    short *val_array;
} Character;

Font loadFont(FILE *f);

Character loadNextChar(FILE *f, Font font);

short getVal(Font *f, Character *c, int row, int col);

void printChar(Character c, Font f);


