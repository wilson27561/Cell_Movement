cmake_minimum_required(VERSION 3.19)
project(Cell_Movement)

set(CMAKE_CXX_STANDARD 14)

add_executable(Cell_Movement main.cpp flute/flute.cpp flute/flute.h flute/flute_mst.cpp flute/dl.h flute/dl.cpp flute/mst2.cpp flute/mst2.h flute/heap.cpp flute/heap.h flute/global.h flute/neighbors.cpp flute/neighbors.h flute/dist.cpp flute/dist.h flute/err.cpp flute/err.h Util/Constant.cpp Util/Constant.h Header/Layer.cpp Header/Layer.h)