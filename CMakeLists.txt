cmake_minimum_required(VERSION 3.19)
project(Cell_Movement)

set(CMAKE_CXX_STANDARD 14)
file(GLOB SRC_LIST main.cpp "flute/*.cpp" "Header/*.cpp" "Util/*.cpp")
add_executable(Cell_Movement ${SRC_LIST} Header/PatternPoint.cpp Header/PatternPoint.h Header/RoutePoint.cpp Header/RoutePoint.h)
#Util/CellMoveRoute.cpp Util/CellMoveRoute.h
#add_executable(Cell_Movement main.cpp flute/flute.cpp flute/flute.h flute/flute_mst.cpp flute/dl.h flute/dl.cpp flute/mst2.cpp flute/mst2.h flute/heap.cpp flute/heap.h flute/global.h flute/neighbors.cpp flute/neighbors.h flute/dist.cpp flute/dist.h flute/err.cpp flute/err.h Util/Constant.cpp Util/Constant.h Header/Layer.cpp Header/Layer.h Util/ReadFile.cpp Header/GgridBoundaryIndex.cpp Header/GgridBoundaryIndex.h Header/NumNonDefaultSupplyGgrid.cpp Header/NumNonDefaultSupplyGgrid.h Header/Pin.cpp Header/Pin.h Header/Blockage.cpp Header/Blockage.h Header/MasterCell.cpp Header/MasterCell.h Header/CellInstance.cpp Header/CellInstance.h Header/Net.cpp Header/Net.h Header/Route.cpp Header/Route.h Header/VoltageArea.cpp Header/VoltageArea.h Header/Grid.cpp Header/Grid.h Util/ReRoute.cpp Header/SteinerPoint.cpp Header/SteinerPoint.h Util/ReRoute.h Util/ReadFile.h)