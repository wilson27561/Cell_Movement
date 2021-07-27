//
// Created by Ｗilson on 2021/5/16.
//

#include "NumNonDefaultSupplyGgrid.h"

int NumNonDefaultSupplyGgrid::getRowIndx() const {
    return rowIndx;
}

void NumNonDefaultSupplyGgrid::setRowIndx(int rowIndx) {
    NumNonDefaultSupplyGgrid::rowIndx = rowIndx;
}

int NumNonDefaultSupplyGgrid::getCollndx() const {
    return collndx;
}

void NumNonDefaultSupplyGgrid::setCollndx(int collndx) {
    NumNonDefaultSupplyGgrid::collndx = collndx;
}

int NumNonDefaultSupplyGgrid::getLayIndx() const {
    return layIndx;
}

void NumNonDefaultSupplyGgrid::setLayIndx(int layIndx) {
    NumNonDefaultSupplyGgrid::layIndx = layIndx;
}

const string &NumNonDefaultSupplyGgrid::getLayerName() const {
    return layerName;
}

void NumNonDefaultSupplyGgrid::setLayerName(const string &layerName) {
    NumNonDefaultSupplyGgrid::layerName = layerName;
}

int NumNonDefaultSupplyGgrid::getIncrOrDecrValue() const {
    return incrOrDecrValue;
}

void NumNonDefaultSupplyGgrid::setIncrOrDecrValue(int incrOrDecrValue) {
    NumNonDefaultSupplyGgrid::incrOrDecrValue = incrOrDecrValue;
}

NumNonDefaultSupplyGgrid::~NumNonDefaultSupplyGgrid() {

}

NumNonDefaultSupplyGgrid::NumNonDefaultSupplyGgrid() {}
