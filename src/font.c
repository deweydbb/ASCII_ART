#include "font.h"
#include "fontInfo.h"

extern const int NUM_BRIGHT_ROW;
extern const int NUM_BRIGHT_COL;

// loads and returns a font from fontInfo.h
Font loadFont() {
    Font font = {pixWidthOfChar, pixHeightOfChar, numCharInFont};
    return font;
}

// given a character and a font, creates the brightness array of a character
// given the number of rows and columns in the brightness array
void setBrightnessChar(Character *ch, Font font, int rows, int cols) {
    // number of pixels per row/column in a section of the brightness array
    int pixPerRow = font.height / rows;
    int pixPerCol = font.width / cols;
    // allocates space on head
    ch->bright_array = malloc(rows * cols * sizeof(double));
    if (ch->bright_array == NULL) {
        printf("malloc failed to create brightness array");
        exit(1);
    }
    // represents the total of the average value of all sectors
    int sectorSum = 0;
    // loops through each section of the brightness array
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {

            int sum = 0;
            // loops through all the pixels in the brightness array
            for (int secRow = 0; secRow < pixPerRow; secRow++) {
                for (int secCol = 0; secCol < pixPerCol; secCol++) {
                    // calculates the corresponding index of the pixel in the pixel array
                    int index = (r * pixPerRow + secRow) * font.width + (c * pixPerCol + secCol);
                    sum += ch->val_array[index];
                }
            }

            sectorSum += sum / (pixPerCol * pixPerRow);

            int indexBright = r * cols + c;
            ch->bright_array[indexBright] = sum / (pixPerCol * pixPerRow);
        }
    }
    // average value of each sector
    double avg = (double) sectorSum / (rows * cols);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int index = r * cols + c;
            // makes brightness array value relative to other values in the
            // brightness array. 1.0 means the sector is the average of all other sectors
            // any values less than 1.0 means the sector is darker than average
            // greater than 1.0 means it is lighter than average
            ch->bright_array[index] /= avg;
        }
    }
}

// loads the next character from fontInfo.h
Character loadNextChar(Font font, int charNum) {
    // extra two, 1 for symbol at beginning of line, one for newline char at end of string
    int len = font.width * font.height + 2;
    int startIndex = len * charNum;

    Character c;
    c.symbol = fontInfo[startIndex];
    // equals sign is special char representing space.
    if (c.symbol == '=') {
        c.symbol = ' ';
    }

    c.val_array = (short *) malloc(len * sizeof(short));
    if (c.val_array == NULL) {
        printf("failed to create value array for char %c", c.symbol);
        exit(1);
    }

    // starts at 1 because first character is the actual char the val_array represents
    for (int i = 1; i < len; i++) {
        // characters should either be 1 or 0
        if (fontInfo[startIndex + i] == '0') {
            // i minus one to offset i starting at 1
            c.val_array[i - 1] = 0;
        } else {
            const short WHITE = 255;
            c.val_array[i - 1] = WHITE;
        }
    }

    setBrightnessChar(&c, font, NUM_BRIGHT_ROW, NUM_BRIGHT_COL);

    return c;
}

// loads all the characters from fontInfo.h and returns a
// pointer to the array of characters
Character *getCharArray(Font font) {
    Character *chars = (Character *) malloc(font.numChar * sizeof(Character));
    for (int i = 0; i < font.numChar; i++) {
        chars[i] = loadNextChar(font, i);
    }

    return chars;
}

// simple linear search to get pointer to character based on symbol
Character *getCharacterFromSymbol(Character *chars, Font font, unsigned char symbol) {
    for (int i = 0; i < font.numChar; i++) {
        if (chars[i].symbol == symbol) {
            return &chars[i];
        }
    }

    return NULL;
}

// gets the color of a character at a specific row and column
short getVal(Font *f, Character *c, int row, int col) {
    int index = (row * f->width) + col;
    return c->val_array[index];
}

// prints the character symbol, pixel array and brightness array
void printChar(Character ch, Font f) {
    printf("Character: %c\n", ch.symbol);

    // prints pixel array
    for (int r = 0; r < f.height; r++) {
        for (int c = 0; c < f.width; c++) {
            int val = getVal(&f, &ch, r ,c);
            if (val == 0) {
                printf("  0  ");
            } else {
                printf(" 255 ");
            }
        }
        printf("\n");
    }
    printf("\n");
    // prints brightness array
    printCharBrightness(ch);
}

// prints just the brightness array of a character
void printCharBrightness(Character ch) {
    // loops through brightness array
    for (int r = 0; r < NUM_BRIGHT_ROW; r++) {
        for (int c = 0; c < NUM_BRIGHT_COL; c++) {
            int index = r * NUM_BRIGHT_COL + c;
            printf("%f ", ch.bright_array[index]);
        }
        printf("\n");
    }
    printf("\n");
}
