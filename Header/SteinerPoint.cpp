//
// Created by Ｗilson on 2021/5/31.
//

#include "SteinerPoint.h"

int SteinerPoint::getSteinerPointRow() const {
    return steinerPointRow;
}

void SteinerPoint::setSteinerPointRow(int steinerPointRow) {
    SteinerPoint::steinerPointRow = steinerPointRow;
}

int SteinerPoint::getSteinerPointCol() const {
    return steinerPointCol;
}

void SteinerPoint::setSteinerPointCol(int steinerPointCol) {
    SteinerPoint::steinerPointCol = steinerPointCol;
}

int SteinerPoint::getCellPointRow() const {
    return CellPointRow;
}

void SteinerPoint::setCellPointRow(int cellPointRow) {
    CellPointRow = cellPointRow;
}

int SteinerPoint::getCellPointCol() const {
    return CellPointCol;
}

void SteinerPoint::setCellPointCol(int cellPointCol) {
    CellPointCol = cellPointCol;
}

SteinerPoint::SteinerPoint(int steinerPointRow, int steinerPointCol, int cellPointRow, int cellPointCol)
        : steinerPointRow(steinerPointRow), steinerPointCol(steinerPointCol), CellPointRow(cellPointRow),
          CellPointCol(cellPointCol) {}

SteinerPoint::SteinerPoint() {}

int SteinerPoint::getLayer() const {
    return layer;
}

void SteinerPoint::setLayer(int layer) {
    SteinerPoint::layer = layer;
}

SteinerPoint::SteinerPoint(int steinerPointRow, int steinerPointCol, int cellPointRow, int cellPointCol, int layer)
        : steinerPointRow(steinerPointRow), steinerPointCol(steinerPointCol), CellPointRow(cellPointRow),
          CellPointCol(cellPointCol), layer(layer) {}

SteinerPoint::~SteinerPoint() {

}
