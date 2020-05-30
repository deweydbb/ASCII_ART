# ASCII_ART
C program to convert images and gifs into ascii art

##How To Use
Each cell of the image that will be converted into a single character is broken up into
8 sections. (2 sections wide, 4 sections tall) SEC_LEN specifies the length of each section. 
For example if SEC_LEN is 2, the section will be made up of a square of 4 pixels (since 2 
squared is 4). This mean that each cell of the image that is 4 pixels wide and 8 pixels wide
will be converted to one character.

    const int SEC_LEN = 2;

    char *IMG = "path to image to convert";
    char *FONT_FILE = "path to font info file";
    char *OUTPUT = "path to text file output";
    
##How To Change Font
To change what font the program uses, you need to create a new fontInfo file and update the
value stored in:

     char *FONT_FILE = "path to font info file";
    
**The font must be monospaced.** 

The format of the fontInfo text file is as follows:
    
    number of characters in the font
    width in pixels of the font
    height in pixels of the font
    [1st character in font][width * height number of 1's or 0's, 1 represents a white pixel
    0 represents a black pixel]\n
    [2nd char][width * height 1's and 0s]\n
    .
    .
    .
    [Nth char][width * height 1's and 0s]\n
    
There is a special case for the character ' ' (space). Because white space is usually used
as a delimiter, I used the equal sign to represent the space character because I am lazy.

## GIFS

Gifs are defiantly still a work in progress. Change the IMG variable to the path of the Gif
you want to use just like you would any image.

However instead of producing a nice Gif output, the program outputs each frame of the GIF
to the imageOutput sub-directory. Each frame is named: frame_0x.jpg, where x is the number
representing the order of the frame so 00 is the first frame, 01 is the second and so on.
There are many online converters that allow you to create a gif from a bunch of images. 
However, I use a command line tool called [ImageMagick](https://imagemagick.org/index.php).
There is a simple bat file createGif.bat that runs the program and then
uses ImageMagick to create the gif.