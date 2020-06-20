#include "font.h"
#include "fontInfo.h"

extern const int NUM_BRIGHT_ROW;
extern const int NUM_BRIGHT_COL;

// loads and returns a font from fontInfo.h
Font loadFont() {
    Font font = {pixWidthOfChar, pixHeightOfChar, numCharInFont};
    return font;
}

double getHorSlope(Character *ch, Font font, int row) {
    double avgSlope = 0;

    for (int col1 = 0; col1 < font.width; col1++) {
        for (int col2 = col1 + 1; col2 < font.width; col2++) {
            int indexBase = row * font.width;
            avgSlope += ch->val_array[indexBase + col1] - ch->val_array[indexBase + col2];
        }
    }

    return avgSlope /= (font.width - 1) * (font.width / 2.0);
}

double getVertSlope(Character *ch, Font font, int col) {
    double avgSlope = 0;

    for (int row1 = 0; row1 < font.height; row1++) {
        for (int row2 = row1 + 1; row2 < font.height; row2++) {
            int index1 = row1 * font.width + col;
            int index2 = row2 * font.width + col;
            avgSlope += ch->val_array[index1] - ch->val_array[index2];
        }
    }

    return avgSlope /= (font.height - 1) * (font.height / 2.0);
}

void setSlopes(Character *ch, Font font) {
    double horSlope = 0;
    double vertSlope = 0;

    for (int row = 0; row < font.height; row++) {
        horSlope += getHorSlope(ch, font, row);
    }

    for (int col = 0; col < font.width; col++) {
        vertSlope += getVertSlope(ch, font, col);
    }

    horSlope /= font.height;
    vertSlope /= font.width;

    ch->dx = horSlope;
    ch->dy = vertSlope;
}

void setAvgBright(Character *ch, Font font) {
    double totalBright = 0;
    for (int i = 0; i < font.width * font.height; i++) {
        totalBright += ch->val_array[i];
    }

    ch->avgBright = totalBright / (font.width * font.height);
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

    setSlopes(&c, font);
    setAvgBright(&c, font);

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
}

