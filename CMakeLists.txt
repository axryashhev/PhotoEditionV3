cmake_minimum_required(VERSION 3.21)
project(PhotoEditionV3)

set(CMAKE_CXX_STANDARD 14)

include_directories(PhotoEditionV3)

add_executable(PhotoEditionV3
        PhotoEditionV3/Functions.h
        PhotoEditionV3/Loader.h
        PhotoEditionV3/main.cpp
        PhotoEditionV3/resource.h
        PhotoEditionV3/WinProc.h)
