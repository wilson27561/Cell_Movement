//
// Created by Wu Wei shou on 2021/8/16.
//

#include "PatternPoint.h"

int PatternPoint::getStartPointRow() const {
    return startPointRow;
}

void PatternPoint::setStartPointRow(int startPointRow) {
    PatternPoint::startPointRow = startPointRow;
}

int PatternPoint::getStartPointCol() const {
    return startPointCol;
}

void PatternPoint::setStartPointCol(int startPointCol) {
    PatternPoint::startPointCol = startPointCol;
}

int PatternPoint::getEndPointRow() const {
    return endPointRow;
}

void PatternPoint::setEndPointRow(int endPointRow) {
    PatternPoint::endPointRow = endPointRow;
}

int PatternPoint::getEndPointCol() const {
    return endPointCol;
}

void PatternPoint::setEndPointCol(int endPointCol) {
    PatternPoint::endPointCol = endPointCol;
}

int PatternPoint::getLayer() const {
    return layer;
}

void PatternPoint::setLayer(int layer) {
    PatternPoint::layer = layer;
}

PatternPoint::PatternPoint() {}

PatternPoint::~PatternPoint() {

}
