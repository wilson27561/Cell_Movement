//
// Created by Ｗilson on 2021/5/17.
//

#include "MasterCell.h"

const string &MasterCell::getMasterCellName() const {
    return masterCellName;
}

void MasterCell::setMasterCellName(const string &masterCellName) {
    MasterCell::masterCellName = masterCellName;
}

const map <string, Pin> &MasterCell::getPinType() const {
    return pinType;
}

void MasterCell::setPinType(const map <string, Pin> &pinType) {
    MasterCell::pinType = pinType;
}

const map <string, Blockage> &MasterCell::getBlockageType() const {
    return blockageType;
}

void MasterCell::setBlockageType(const map <string, Blockage> &blockageType) {
    MasterCell::blockageType = blockageType;
}

MasterCell::~MasterCell() {

}

MasterCell::MasterCell() {}
