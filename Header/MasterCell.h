//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_MASTERCELL_H
#define CELL_MOVEMENT_MASTERCELL_H

#include <iostream>
#include <map>
#include "Pin.h"
#include "Blockage.h"
using namespace std;
class Pin;
class Blockage;
class MasterCell {
private :
    string masterCellName;
    map<string,Pin> pinType;
    map<string,Blockage> blockageType;
public:
    const string &getMasterCellName() const;

    void setMasterCellName(const string &masterCellName);

    const map<string, Pin> &getPinType() const;

    void setPinType(const map<string, Pin> &pinType);

    const map<string, Blockage> &getBlockageType() const;

    void setBlockageType(const map<string, Blockage> &blockageType);

};


#endif //CELL_MOVEMENT_MASTERCELL_H
