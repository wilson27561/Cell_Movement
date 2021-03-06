//
// Created by Ｗilson on 2021/5/17.
//

#include "CellInstance.h"

const string &CellInstance::getCellName() const {
    return cellName;
}

void CellInstance::setCellName(const string &cellName) {
    CellInstance::cellName = cellName;
}


int CellInstance::getRowIndx() const {
    return rowIndx;
}

void CellInstance::setRowIndx(int rowIndx) {
    CellInstance::rowIndx = rowIndx;
}

int CellInstance::getColIndx() const {
    return colIndx;
}

void CellInstance::setColIndx(int colIndx) {
    CellInstance::colIndx = colIndx;
}

const string &CellInstance::getMovalbe() const {
    return movalbe;
}

void CellInstance::setMovalbe(const string &movalbe) {
    CellInstance::movalbe = movalbe;
}

const string &CellInstance::getMasterCellName() const {
    return masterCellName;
}

void CellInstance::setMasterCellName(const string &masterCellName) {
    CellInstance::masterCellName = masterCellName;
}

int CellInstance::getLayerName() const {
    return layerName;
}

void CellInstance::setLayerName(int layerName) {
    CellInstance::layerName = layerName;
}


CellInstance::~CellInstance() {

}

CellInstance::CellInstance() {}

const vector<string> &CellInstance::getConnectNetVector() const {
    return connectNetVector;
}

void CellInstance::setConnectNetVector(const vector<string> &connectNetVector) {
    CellInstance::connectNetVector = connectNetVector;
}
