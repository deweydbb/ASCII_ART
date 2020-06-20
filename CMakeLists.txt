cmake_minimum_required(VERSION 3.16)
project(ASCII_ART C)

set(CMAKE_C_STANDARD 11)

add_executable(ASCII_ART src/main.c libs/stb_image.h libs/stb_image_write.h src/Font.c src/Font.h src/Cell.c src/Cell.h src/Image.c src/Image.h libs/gifenc.h libs/gifenc.c src/File.c src/File.h libs/tinyfiledialogs.h libs/tinyfiledialogs.c src/FontInfo.h)