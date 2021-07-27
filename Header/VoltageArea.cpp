//
// Created by Ｗilson on 2021/5/17.
//

#include "VoltageArea.h"

const string &VoltageArea::getAreaName() const {
    return areaName;
}

void VoltageArea::setAreaName(const string &areaName) {
    VoltageArea::areaName = areaName;
}

const vector<Grid> &VoltageArea::getGridVector() const {
    return gridVector;
}

void VoltageArea::setGridVector(const vector<Grid> &gridVector) {
    VoltageArea::gridVector = gridVector;
}

const vector<string> &VoltageArea::getInstance() const {
    return instance;
}

void VoltageArea::setInstance(const vector<string> &instance) {
    VoltageArea::instance = instance;
}

VoltageArea::~VoltageArea() {

}

VoltageArea::VoltageArea() {}
