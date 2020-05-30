#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

// returns a struct that contains an array of pixels, width and height of an image
// in grey scale. Exits program if unable to load image.
Image *getImage(char *path) {
    int width, height, channels;
    unsigned char *pixels = stbi_load(path, &width, &height, &channels, 1);

    // failed to load image, exit program
    if (pixels == NULL) {
        printf("Error in loading image\n");
        exit(1);
    }
    // create image struct
    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->pix = pixels;

    return img;
}

// frees up the memory on the heap used to store the image
void freeImage(Image *image) {
    stbi_image_free(image->pix);
}

Image *createPixelResult(const unsigned char *charResult, Character *chars, Font font, int charPerRow, int charPerCol) {
    Image *result = malloc(sizeof(Image));

    result->width = font.width * charPerCol;
    result->height = font.height * charPerRow;

    result->pix = malloc(result->width * result->height * sizeof(char));

    int index = 0;

    for (int row = 0; row < charPerRow; row++) {
        for (int fontRow = 0; fontRow < font.height; fontRow++) {
            for (int col = 0; col < charPerCol; col++) {
                unsigned char currentChar = charResult[row * charPerCol + col];
                Character *ch = getCharacterFromSymbol(chars, font, currentChar);
                for (int fontCol = 0; fontCol < font.width; fontCol++) {
                    int val_arrayIndex = fontRow * font.width + fontCol;
                    unsigned char val = ch->val_array[val_arrayIndex];

                    result->pix[index] = val;
                    index++;
                }
            }
        }
    }

    return result;
}

void createJpgOfResult(unsigned char *charResult, Character *chars, Font font, int charPerRow, int charPerCol, int fileNum) {
    Image *img = createPixelResult(charResult, chars, font, charPerRow, charPerCol);

    char ch[80];
    if (fileNum == -1) {
        sprintf(ch, "D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/results.jpg");
    } else {
        sprintf(ch, "D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/imageOutput/frame_0%d.jpg", fileNum);
    }
    stbi_write_jpg(ch, img->width, img->height, 1,
                   img->pix, img->width);

    free(img->pix);
}

Gif *getGif(const char *filename) {
    FILE *gif = fopen(filename, "rb");
    if (gif == NULL) {
        printf("failed to load gif");
        exit(1);
    }

    fseek(gif, 0L, SEEK_END);
    int size = (int) ftell(gif);
    void *buffer = malloc(size);
    fseek(gif, 0L, SEEK_SET);
    fread(buffer, size, 1, gif);
    fclose(gif);

    int *delays = NULL;
    int width, height, frames, comp;
    int req_comp = 1;

    stbi_uc *loadedGif = stbi_load_gif_from_memory(buffer, size, &delays, &width, &height, &frames, &comp,
                                                         req_comp);
    free(buffer);

    if (loadedGif == NULL) {
        printf("failed to load gif");
        exit(1);
    }

    Gif *result = malloc(sizeof(Gif));
    result->width = width;
    result->height = height;
    result->numFrames = frames;
    result->delay = *delays;
    result->pix = loadedGif;

    return result;
}

void writeGif(Gif *gif) {
    char ch[80];
    for (int i = 0; i < gif->numFrames; i++) {
        sprintf(ch, "D:/System Folders/Desktop/UT/Summer 2020/ASCII_ART/imageOutput/frame_0%d.jpg", i);
        stbi_write_jpg(ch, gif->width, gif->height, 1, gif->pix + gif->width * gif->height * i, 0);
    }
}
