//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_VOLTAGEAREA_H
#define CELL_MOVEMENT_VOLTAGEAREA_H
#include "Grid.h"
#include <iostream>
#include <vector>
using namespace std;
class VoltageArea {
private:
    string areaName;
    vector<Grid> gridVector;
    vector<string> instance;
public:
    virtual ~VoltageArea();

    VoltageArea();

public:
    const string &getAreaName() const;

    void setAreaName(const string &areaName);

    const vector<Grid> &getGridVector() const;

    void setGridVector(const vector<Grid> &gridVector);

    const vector<string> &getInstance() const;

    void setInstance(const vector<string> &instance);


};


#endif //CELL_MOVEMENT_VOLTAGEAREA_H
