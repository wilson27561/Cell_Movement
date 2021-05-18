//
// Created by Ｗilson on 2021/5/15.
//

#include "GgridBoundaryIndex.h"

int GgridBoundaryIndex::getRowBeginIdx() const {
    return rowBeginIdx;
}

void GgridBoundaryIndex::setRowBeginIdx(int rowBeginIdx) {
    GgridBoundaryIndex::rowBeginIdx = rowBeginIdx;
}

int GgridBoundaryIndex::getColBeginIdx() const {
    return colBeginIdx;
}

void GgridBoundaryIndex::setColBeginIdx(int colBeginIdx) {
    GgridBoundaryIndex::colBeginIdx = colBeginIdx;
}

int GgridBoundaryIndex::getRowEndIdx() const {
    return rowEndIdx;
}

void GgridBoundaryIndex::setRowEndIdx(int rowEndIdx) {
    GgridBoundaryIndex::rowEndIdx = rowEndIdx;
}

int GgridBoundaryIndex::getColEndIdx() const {
    return colEndIdx;
}

void GgridBoundaryIndex::setColEndIdx(int colEndIdx) {
    GgridBoundaryIndex::colEndIdx = colEndIdx;
}
