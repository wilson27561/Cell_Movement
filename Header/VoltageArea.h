//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_VOLTAGEAREA_H
#define CELL_MOVEMENT_VOLTAGEAREA_H
#include "Grid.h"
#include <unordered_map>
#include <iostream>
#include <vector>
using namespace std;
class VoltageArea {
private:
    string areaName;
    //row_col
    unordered_map<string,string> gridMap;
public:
    const string &getAreaName() const;

    void setAreaName(const string &areaName);

    const unordered_map<string, string> &getGridMap() const;

    void setGridMap(const unordered_map<string, string> &gridMap);


};


#endif //CELL_MOVEMENT_VOLTAGEAREA_H
