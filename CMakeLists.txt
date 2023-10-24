cmake_minimum_required(VERSION 3.5)

project(SMA LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SMA main.cpp
    series.h series.cpp
    sma.h sma.cpp
    macd.h macd.cpp
    utils.h utils.cpp
)
