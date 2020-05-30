cmake_minimum_required(VERSION 3.16)
project(ASCII_ART C)

set(CMAKE_C_STANDARD 11)

add_executable(ASCII_ART main.c stb_image.h stb_image_write.h font.c font.h Cell.c Cell.h Image.c Image.h)