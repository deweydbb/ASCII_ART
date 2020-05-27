#include <stdio.h>
#include <stdlib.h>
#include "font.h"

Font loadFont(FILE *f) {
    Font font;

    fscanf(f, "%i\n", &(font.numChar));
    fscanf(f, "%d\n", &font.width);
    fscanf(f, "%d\n", &font.height);

    return font;
}

Character loadNextChar(FILE *f, Font font) {
    int len = font.width * font.height;
    // extra two, 1 for symbol at beginning of line, one for null char at end of string
    char line[len + 2];

    fscanf(f, "%s\n", line);

    Character c;

    c.symbol = line[0];
    // equals sign is special char representing space.
    if (c.symbol == '=') {
        c.symbol = ' ';
    }
    // allocate memory for value array, not necessary to free ever
    // because values need to persist for running of program
    c.val_array = (short *) malloc(len * sizeof(short));
    if (c.val_array == NULL) {
        printf("failed to create value array for char %c", c.symbol);
        exit(1);
    }

    // starts at 1 because first character is the actual char the val_array represents
    for (int i = 1; i < len + 1; i++) {
        // characters should either be 1 or 0
        if (line[i] == '0') {
            // i minus one to offset i starting at 1
            c.val_array[i - 1] = 0;
        } else {
            c.val_array[i - 1] = 255;
        }
    }

    return c;
}

short getVal(Font *f, Character *c, int row, int col) {
    int index = (row * f->width) + col;
    return c->val_array[index];
}

void printChar(Character ch, Font f) {
    printf("Character: %c\n", ch.symbol);

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
