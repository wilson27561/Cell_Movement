cmake_minimum_required(VERSION 3.19)
project(Cell_Movement)

set(CMAKE_CXX_STANDARD 14)

add_executable(Cell_Movement main.cpp flute.cpp flute.h flute_mst.cpp dl.h dl.cpp mst2.cpp mst2.h heap.cpp heap.h global.h neighbors.cpp neighbors.h dist.cpp dist.h err.cpp err.h)