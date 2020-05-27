#include <stdio.h>
#include <stdlib.h>
#include "Cell.h"


Character *getCharArray(Font font, FILE *f) {
    Character *chars = (Character *) malloc(font.numChar * sizeof(Character));
    for (int i = 0; i < font.numChar; i++) {
        chars[i] = loadNextChar(f, font);
    }

    return chars;
}

FILE *openFile(char *path, char *mode) {
    FILE *f = fopen(path, mode);

    if (f == NULL) {
        printf("failed to load file\n");
        exit(1);
    }

    return f;
}

int main() {
      FILE *fontInfo = openFile("D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/fontInfo.txt", "r");
      Font font = loadFont(fontInfo);
      Character *chars = getCharArray(font, fontInfo);
      fclose(fontInfo);

      //TODO free image
      Image *image = getImage("D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/testImage3.jpg");

      printf("image width: %d, height: %d\n", image->width, image->height);

      FILE *result = openFile("D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/result.txt", "w");

      const int ratio = 1;
      int numCellsPerRow = image->height / (font.height * ratio);
      int numCellsPerCol = image->width / (font.width * ratio);

      for (int row = 0; row < numCellsPerRow; row++) {
          for (int col = 0; col < numCellsPerCol; col++) {
              Cell c = getCell(image, row, col, font, ratio);

              Character bestChar = getBestChar(c, chars, font);
              fprintf(result, "%c", bestChar.symbol);

              free(c.val_array);
          }

          fprintf(result, "\n");
      }

    return 0;
}


