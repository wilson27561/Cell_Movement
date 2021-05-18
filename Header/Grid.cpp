//
// Created by Ｗilson on 2021/5/17.
//

#include "Grid.h"

int Grid::getRowIndx() const {
    return rowIndx;
}

void Grid::setRowIndx(int rowIndx) {
    Grid::rowIndx = rowIndx;
}

int Grid::getColIndx() const {
    return colIndx;
}

void Grid::setColIndx(int colIndx) {
    Grid::colIndx = colIndx;
}

int Grid::getSupply() const {
    return supply;
}

void Grid::setSupply(int supply) {
    Grid::supply = supply;
}
