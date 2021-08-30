//
// Created by Ｗilson on 2021/5/17.
//

#include "VoltageArea.h"
#include <unordered_map>

const string &VoltageArea::getAreaName() const {
    return areaName;
}

void VoltageArea::setAreaName(const string &areaName) {
    VoltageArea::areaName = areaName;
}

const unordered_map<string, string> &VoltageArea::getGridMap() const {
    return gridMap;
}

void VoltageArea::setGridMap(const unordered_map<string, string> &gridMap) {
    VoltageArea::gridMap = gridMap;
}
