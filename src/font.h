#include <stdio.h>
#include <stdlib.h>


#ifndef ASCII_ART_FONT_H
#define ASCII_ART_FONT_H

#endif //ASCII_ART_FONT_H
// font struct includes width and height of a character in pixels
// stores number of characters in the font
typedef struct {
    int width;
    int height;
    int numChar;
} Font;

// stores a specific character and its associated
// pixel array and brightness array
typedef struct {
    char symbol;
    short *val_array;
    double *bright_array;
} Character;

// returns a font struct based on a file
Font loadFont();

// gets all the characters in the font
Character *getCharArray(Font font);

// simple linear search to get pointer to character based on symbol
Character *getCharacterFromSymbol(Character *chars, Font font, unsigned char symbol);

// given a row and column returns a value between 0 and 255
// represents the color at that specific pixel of a character
short getVal(Font *f, Character *c, int row, int col);

// prints a character, its pixel array and its brightness array
void printChar(Character c, Font f);

// prints just the brightness array of a character
void printCharBrightness(Character c);


