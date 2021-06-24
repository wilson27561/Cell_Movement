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


int Blockage::getDemand() const {
    return demand;
}

void Blockage::setDemand(int demand) {
    Blockage::demand = demand;
}

int Blockage::getBlockageLayer() const {
    return blockageLayer;
}

void Blockage::setBlockageLayer(int blockageLayer) {
    Blockage::blockageLayer = blockageLayer;
}
