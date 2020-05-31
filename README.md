# ASCII_ART
C program to convert images and gifs into ascii art

##How To Use
Each cell of the image that will be converted into a single character is broken up into
8 sections. (2 sections wide, 4 sections tall) SEC_LEN specifies the length of each section. 
For example if SEC_LEN is 2, the section will be made up of a square of 4 pixels (since 2 
squared is 4). This mean that each cell of the image that is 4 pixels wide and 8 pixels wide
will be converted to one character.

    const int SEC_LEN = 2;

    char *IMG = "path to image (or gif) to convert";
    char *FONT_FILE = "path to font info file";
    char *OUTPUT = "path to text file output";
    char *GIF_OUTPUT = "path to gif file output";
    char *IMG_OUTPUT = "path to img file output";
    
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
The current font used in the project is Consolas Regular simply because it is the default
font for windows notepad. 

## GIFS
Change the `char *IMG` variable to the path of the GIF you want to convert. The program will write to
whatever the file whose path is stored in `char *GIF_OUTPUT` 
