@ECHO OFF
cd imageOutput
del *.jpg
del *.gif
cd ..
gcc -o ascii main.c Cell.c Cell.h font.c font.h Image.c Image.h stb_image.h stb_image_write.h
ascii
del ascii.exe
cd imageOutput
magick convert -delay 10 -loop 0 *.jpg output.gif
cd ..