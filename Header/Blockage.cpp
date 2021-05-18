//
// Created by Ｗilson on 2021/5/17.
//

#include "Blockage.h"

const string &Blockage::getBlockageName() const {
    return blockageName;
}

void Blockage::setBlockageName(const string &blockageName) {
    Blockage::blockageName = blockageName;
}

const string &Blockage::getBlockageLayer() const {
    return blockageLayer;
}

void Blockage::setBlockageLayer(const string &blockageLayer) {
    Blockage::blockageLayer = blockageLayer;
}

int Blockage::getDemand() const {
    return demand;
}

void Blockage::setDemand(int demand) {
    Blockage::demand = demand;
}
