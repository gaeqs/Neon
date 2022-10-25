cmake_minimum_required(VERSION 3.0)
project(stb)

add_library(stb STATIC src/stb_image.h src/dummy.cpp)
target_include_directories(stb PUBLIC src/)