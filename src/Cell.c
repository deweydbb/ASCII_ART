#include "Cell.h"


extern const int NUM_BRIGHT_ROW;
extern const int NUM_BRIGHT_COL;

double getHorSlopeCell(Cell *cell, int row) {
    double avgSlope = 0;

    for (int col1 = 0; col1 < cell->width; col1++) {
        for (int col2 = col1 + 1; col2 < cell->width; col2++) {
            int indexBase = row * cell->width;
            avgSlope += cell->val_array[indexBase + col1] - cell->val_array[indexBase + col2];
        }
    }

    return avgSlope /= (cell->width - 1) * (cell->width / 2.0);
}

double getVertSlopeCell(Cell *cell, int col) {
    double avgSlope = 0;

    for (int row1 = 0; row1 < cell->height; row1++) {
        for (int row2 = row1 + 1; row2 < cell->height; row2++) {
            int index1 = row1 * cell->width + col;
            int index2 = row2 * cell->width + col;
            avgSlope += cell->val_array[index1] - cell->val_array[index2];
        }
    }

    return avgSlope /= (cell->height - 1) * (cell->height / 2.0);
}

void setSlopesCell(Cell *cell) {
    double horSlope = 0;
    double vertSlope = 0;

    for (int row = 0; row < cell->height; row++) {
        horSlope += getHorSlopeCell(cell, row);
    }

    for (int col = 0; col < cell->width; col++) {
        vertSlope += getVertSlopeCell(cell, col);
    }

    horSlope /= cell->height;
    vertSlope /= cell->width;

    cell->dx = horSlope;
    cell->dy = vertSlope;
}

void setAvgBrightCell(Cell *cell) {
    double totalBright = 0;
    int numPix = cell->width * cell->height;

    for (int i = 0; i < numPix; i++) {
        totalBright += cell->val_array[i];
    }

    cell->avgBright = totalBright / numPix;
}

// returns a cell that represents the cell at a given row and column of an image
Cell getCell(Image *image, int cellRow, int cellCol, int secLen) {
    int startRow = cellRow * NUM_BRIGHT_ROW * secLen;
    int startCol = cellCol * NUM_BRIGHT_COL * secLen;

    Cell c;
    c.width = secLen * NUM_BRIGHT_COL;
    c.height = secLen * NUM_BRIGHT_ROW;
    // allocates space for pixel array on head
    c.val_array = malloc(c.width * c.height * sizeof(short));
    if (c.val_array == NULL) {
        printf("malloc failed to create pixel array");
        exit(1);
    }

    // loop through all corresponding pixels in the image that the cell represents
    for (int row = 0; row < c.height; row++) {
        for (int col = 0; col < c.width; col++) {
            int index = row * c.width + col;

            int pixIndex = (startRow + row) * image->width + (startCol + col);

            c.val_array[index] = image->pix[pixIndex];
        }
    }
    // sets the brightness array of the cell
    setSlopesCell(&c);
    setAvgBrightCell(&c);

    return c;
}

double makePositive(double input) {
    if (input < 0) {
        return input * -1;
    }

    return input;
}

// helper method for getBestChar
// compares a cell to a character and returns a double representing
// how close of a match the cell and character are. 0 is a perfect
// match, the larger the number the less similar they are.
double compareCellToChar(Cell cell, Character ch) {
    double horDiff = ch.dx - cell.dx;
    double vertDiff = ch.dy - cell.dy;
    double brightDiff = ch.avgBright - cell.avgBright;
    horDiff = makePositive(horDiff);
    vertDiff = makePositive(vertDiff);
    brightDiff = makePositive(brightDiff);

    return vertDiff + horDiff + (brightDiff / 15);
}

// returns the character that is the best match for cell.
Character getBestChar(Cell cell, Character *chars, Font font) {
    int bestIndex = 0;
    double lowestDiff = 9007199254740992; // max double

    // loops through all of the possible characters
    for (int i = 0; i < font.numChar; i++) {
        double diff = compareCellToChar(cell, chars[i]);
        // if diff is lower than previous best update
        // lowestDiff and best index
        if (diff < lowestDiff) {
            lowestDiff = diff;
            bestIndex = i;
        }
    }

    return chars[bestIndex];
}



