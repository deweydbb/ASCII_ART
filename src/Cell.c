#include "Cell.h"


extern const int NUM_BRIGHT_ROW;
extern const int NUM_BRIGHT_COL;

// helper method for getCell
// given a cell and a font, creates the brightness array of a cell
// given the number of rows and columns in the brightness array
void setBrightness(Cell *cell, int rows, int cols) {
    int pixPerRow = cell->height / rows;
    int pixPerCol = cell->width / cols;
    // allocates space on head for brightness array
    cell->bright_array = malloc(rows * cols * sizeof(double));
    if (cell->bright_array == NULL) {
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
                    int index = (r * pixPerRow + secRow) * cell->width + (c * pixPerCol + secCol);
                    sum += cell->val_array[index];
                }
            }

            sectorSum += sum / (pixPerCol * pixPerRow);

            int indexBright = r * cols + c;
            cell->bright_array[indexBright] = sum / (pixPerCol * pixPerRow);
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
            cell->bright_array[index] /= avg;
        }
    }
}

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
    setBrightness(&c, NUM_BRIGHT_ROW, NUM_BRIGHT_COL);
    setSlopesCell(&c);

    return c;
}

// helper method for getBestChar
// compares a cell to a character and returns a double representing
// how close of a match the cell and character are. 0 is a perfect
// match, the larger the number the less similar they are.
double compareCellToChar(Cell cell, Character ch) {
//    double totalDiff = 0;
//
//    // loops through brightness array of cell and character
//    for (int row = 0; row < NUM_BRIGHT_ROW - 1; row++) {
//        for (int col = 0; col < NUM_BRIGHT_COL; col++) {
//            int index = row * NUM_BRIGHT_COL + col;
//            double diff = cell.bright_array[index] - ch.bright_array[index];
//            // absolute value of difference
//            if (diff < 0) {
//                diff *= -1;
//            }
//
//            totalDiff += diff;
//        }
//    }
//
//    return totalDiff;

    double horDiff = ch.dx - cell.dx;
    double vertDiff = ch.dy - cell.dy;
    if (horDiff < 0) {
        horDiff *= -1;
    }

    if (vertDiff < 0) {
        vertDiff *= -1;
    }

    return vertDiff + horDiff;
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

// prints the brightness array of a cell
// used for debugging
void printCellBrightness(Cell cell) {
    for (int r = 0; r < NUM_BRIGHT_ROW; r++) {
        for (int c = 0; c < NUM_BRIGHT_COL; c++) {
            int index = r * NUM_BRIGHT_COL + c;
            printf("%f ", cell.bright_array[index]);
        }
        printf("\n");
    }
    printf("\n");
}



