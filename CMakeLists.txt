cmake_minimum_required(VERSION 3.17)
project(SequenceProcessing_CPP)

set(CMAKE_CXX_STANDARD 17)

add_executable(SequenceProcessing_CPP
        src/main.cpp
        src/BPE.cpp
        src/BPE.h
)