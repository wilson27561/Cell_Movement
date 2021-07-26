//
// Created by Ｗilson on 2021/5/15.
//

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "../Header/GgridBoundaryIndex.h"
#include "../Header/Layer.h"
#include "../Header/MasterCell.h"
#include "../Header/NumNonDefaultSupplyGgrid.h"
#include "../Header/CellInstance.h"
#include "../Header/VoltageArea.h"
#include "../Util/ReadFile.h"
#include "../Header/Net.h"
#include "Constant.h"

using namespace std;

class GgridBoundaryIndex;

class Util;

//class ReadFile {
//public :
int ReadFile::readMaxCell(vector<string> lineVector) {
    return atoi(lineVector[1].c_str());
}

GgridBoundaryIndex ReadFile::readGGridBoundaryIdx(vector<string> lineVector) {
    GgridBoundaryIndex gridBoundaryIndex;
    gridBoundaryIndex.setRowBeginIdx(atoi(lineVector[1].c_str()));
    gridBoundaryIndex.setColBeginIdx(atoi(lineVector[2].c_str()));
    gridBoundaryIndex.setRowEndIdx(atoi(lineVector[3].c_str()));
    gridBoundaryIndex.setColEndIdx(atoi(lineVector[4].c_str()));
    return gridBoundaryIndex;
}

void ReadFile::readLayer(vector<string> *contentVector, int *index, string layerCount, map<string, Layer> *layerMap) {
    int indexCount = *index;
    int mapSize = indexCount + atoi(layerCount.c_str());
    for (int i = indexCount + 1; i <= mapSize; i++) {
        vector<string> lineVector = splitString((*contentVector)[i]);
        Layer layer;
        layer.setLayerName(lineVector[1]);
        layer.setIndex(stoi(lineVector[2]));
        layer.setRoutingDirection(lineVector[3]);
        layer.setDefaultSupplyOfOneGrid(stoi(lineVector[4]));
        layer.setPowerFactor(stod(lineVector[5]));
        (*layerMap).insert(pair<string, Layer>(lineVector[2], layer));
    };
    *index = mapSize;
}

void ReadFile::getLayerFacotr(map<string, Layer> *layerMap, map<string, vector<int>> *powerFactorMap) {
    vector<int> horizontalVector;
    vector<int> verticalVector;
    vector<double> H_PowerFactorVec;
    vector<double> V_PowerFactorVec;
    for (auto const &item : (*layerMap)) {
        Layer layer = item.second;
        if (layer.getRoutingDirection() == HORIZONTAL) {
            int index = getInsertIndex(H_PowerFactorVec, item.second.getPowerFactor());
            H_PowerFactorVec.insert(H_PowerFactorVec.begin() + index, item.second.getPowerFactor());
            horizontalVector.insert(horizontalVector.begin() + index, item.second.getIndex());
        } else {
            int index = getInsertIndex(V_PowerFactorVec, item.second.getPowerFactor());
            V_PowerFactorVec.insert(V_PowerFactorVec.begin() + index, item.second.getPowerFactor());
            verticalVector.insert(verticalVector.begin() + index, item.second.getIndex());
        }
    }
    (*powerFactorMap).insert(pair<string, vector<int>>(HORIZONTAL, horizontalVector));
    (*powerFactorMap).insert(pair<string, vector<int>>(VERTICAL, verticalVector));

};

void
ReadFile::readNumNonDefaultSupply(vector<string> *contentVector, vector<vector<vector<int> > > *gridVector, int *index,
                                  string layerCount) {
    int indexCount = *index;
    int mapSize = indexCount + stoi(layerCount);
    //  use vector to put NumNonDefaultSupplyGgrid
    for (int i = indexCount + 1; i <= mapSize; i++) {
        vector<string> lineVector = splitString((*contentVector)[i]);
        (*gridVector)[stoi(lineVector[2]) - 1][stoi(lineVector[0]) - 1][stoi(lineVector[1]) - 1] =
                (*gridVector)[stoi(lineVector[2]) - 1][stoi(lineVector[0]) - 1][stoi(lineVector[1]) - 1] +
                stoi(lineVector[3]);
    }
    *index = mapSize;

}


void
ReadFile::readMasterCell(vector<string> *contentVector, vector<string> *lineVector,
                         map<string, MasterCell> *masterCellMap,
                         int *index) {
    int indexCount = *index;
    string masterCellName = (*lineVector)[1];
    int pinCount = stoi((*lineVector)[2]);
    int blockageCount = stoi((*lineVector)[3]);

    MasterCell masterCell;
    masterCell.setMasterCellName(masterCellName);
    map<string, Pin> pinMap;
    map<string, Blockage> blockageMap;
    //PinCount
    for (int i = indexCount + 1; i <= indexCount + pinCount; i++) {
        vector<string> pinVector = splitString((*contentVector)[i]);
        Pin pin;
        pin.setPinName(pinVector[1]);
        pin.setLayer(pinVector[2]);
        pinMap.insert(pair<string, Pin>(pinVector[1], pin));
        masterCell.setPinType(pinMap);
    }
    //BlockageCount
    for (int i = indexCount + pinCount + 1; i <= indexCount + pinCount + blockageCount; i++) {
        vector<string> blockageVector = splitString((*contentVector)[i]);
        Blockage blockage;
        blockage.setBlockageName(blockageVector[1]);
        blockageVector[2].erase(std::remove(blockageVector[2].begin(), blockageVector[2].end(), 'M'),
                                blockageVector[2].end());
        blockage.setBlockageLayer(stoi(blockageVector[2]));
        blockage.setDemand(stoi(blockageVector[3]));
        blockageMap.insert(pair<string, Blockage>(blockageVector[1], blockage));
        masterCell.setBlockageType(blockageMap);
    }
    (*masterCellMap).insert(pair<string, MasterCell>(masterCellName, masterCell));
    *index = indexCount + pinCount + blockageCount;

}

void ReadFile::readCellInstance(vector<string> lineVector, map<string, CellInstance> *cellInstanceMap,
                                map<string, MasterCell> *masterCellMap,
                                map<string, map<string, Blockage>> *blockageCellMap,
                                vector<vector<vector<int> > > *gridVector) {
    CellInstance cellInstance;
    cellInstance.setCellName(lineVector[1]);
    cellInstance.setMasterCellName(lineVector[2]);
    cellInstance.setRowIndx(stoi(lineVector[3]));
    cellInstance.setColIndx(stoi(lineVector[4]));
    cellInstance.setMovalbe(lineVector[5]);
    (*cellInstanceMap).insert(pair<string, CellInstance>(lineVector[1], cellInstance));

    if ((*masterCellMap)[lineVector[2]].getBlockageType().size() > 0) {
        map<string, Blockage> blockageMap = (*masterCellMap)[lineVector[2]].getBlockageType();
        (*blockageCellMap).insert(pair<string, map<string, Blockage>>(lineVector[1], blockageMap));
        // reduce blockage
        for (auto const item : blockageMap) {
            (*gridVector)[item.second.getBlockageLayer() - 1][cellInstance.getRowIndx() - 1][
                    cellInstance.getColIndx() - 1] =
                    (*gridVector)[item.second.getBlockageLayer() - 1][cellInstance.getRowIndx() - 1][
                            cellInstance.getColIndx() - 1] - item.second.getDemand();
        }

    }

};

void ReadFile::readBoundary(CellInstance cellInstance, map<string, int> *boundaryMap) {

    if ((*boundaryMap).find(UP) == (*boundaryMap).end()) {
        (*boundaryMap).insert(pair<string, int>(UP, cellInstance.getRowIndx()));
    } else {
        int oriUp = (*boundaryMap)[UP];
        int newUp = cellInstance.getRowIndx();
        if (newUp > oriUp) {
            (*boundaryMap)[UP] = cellInstance.getRowIndx();

        }
    }
    if ((*boundaryMap).find(DOWN) == (*boundaryMap).end()) {
        (*boundaryMap).insert(pair<string, int>(DOWN, cellInstance.getRowIndx()));
    } else {
        int oriDown = (*boundaryMap)[DOWN];
        int newDown = cellInstance.getRowIndx();
        if (oriDown > newDown) {
            (*boundaryMap)[DOWN] = cellInstance.getRowIndx();
        }
    }
    //做邊
    if ((*boundaryMap).find(LEFT) == (*boundaryMap).end()) {
        (*boundaryMap).insert(pair<string, int>(LEFT, cellInstance.getColIndx()));
    } else {
        int oriLeft = (*boundaryMap)[LEFT];
        int newLeft = cellInstance.getColIndx();
        if (oriLeft > newLeft) {
            (*boundaryMap)[LEFT] = cellInstance.getColIndx();
        }
    }
    if ((*boundaryMap).find(RIGHT) == (*boundaryMap).end()) {
        (*boundaryMap).insert(pair<string, int>(RIGHT, cellInstance.getColIndx()));
    } else {
        int oriRight = (*boundaryMap)[RIGHT];
        int newRight = cellInstance.getColIndx();
        if (newRight > oriRight) {
            (*boundaryMap)[RIGHT] = cellInstance.getColIndx();
        }
    }
}

void
ReadFile::readNet(vector<string> *contentVector, vector<string> *lineVector, map<string, Net> *netMap,
                  map<string, MasterCell> *masterCellMap, map<string, CellInstance> *cellInstanceMap,
                  set<string> *netNameSet, int *index) {
    int indexCount = *index;
    cout << "read Net Name : " << (*lineVector)[1] << endl;
    int pinCount = stoi((*lineVector)[2]);
    Net net;
    net.setNetName((*lineVector)[1]);
    net.setMinRoutingConstraint((*lineVector)[3]);
    net.setWeight(stod((*lineVector)[4]));
    map<string, int> boundaryMap;

    vector<CellInstance> connectCellVector;
    for (int i = indexCount + 1; i <= indexCount + pinCount; i++) {
        vector<string> pinVector = splitString((*contentVector)[i]);
        vector<string> pinCellVec = splitByChar(pinVector[1], '/');
        //pinCellVec[0] -> C1 pinCellVec[1] -> P1 一個net可能會連接同一個cell 不同  pin
        string masterCellName = (*cellInstanceMap)[pinCellVec[0]].getMasterCellName();
        map<string, Pin> pinMap = (*masterCellMap)[masterCellName].getPinType();
        CellInstance cellinstance;
        cellinstance.setCellName(pinCellVec[0]);
//            cellinstance.setConnectPin(pinCellVec[1]);
//            cout << "cell Name : " << pinCellVec[0] <<" Pin :" << pinCellVec[1] << " layer : "  <<pinMap[pinCellVec[1]].getLayer()<< endl;
        string layer = pinMap[pinCellVec[1]].getLayer();
        layer.erase(std::remove(layer.begin(), layer.end(), 'M'), layer.end());
        cellinstance.setLayerName(stoi(layer));
        cellinstance.setRowIndx((*cellInstanceMap)[pinCellVec[0]].getRowIndx());
        cellinstance.setColIndx((*cellInstanceMap)[pinCellVec[0]].getColIndx());
        readBoundary(cellinstance, &boundaryMap);
        connectCellVector.push_back(cellinstance);
    }
    net.setBoundaryMap(boundaryMap);
    net.setConnectCell(connectCellVector);
    (*netMap).insert(pair<string, Net>((*lineVector)[1], net));
    (*netNameSet).insert((*lineVector)[1]);
    (*index) = indexCount + pinCount;
}

void
ReadFile::readRoute(vector<string> *contentVector, vector<string> *lineVector, map<string, Net> *netMap,
                    vector<vector<vector<int> > > *gridVector, int *index, map<string, set<string>> *reducePointMap,
                    set<string> *netNameSet) {
    int indexCount = *index;
    int numRoutes = stoi((*lineVector)[1]);

    for (int i = indexCount + 1; i <= indexCount + numRoutes; i++) {
        vector<string> routeVector = splitString((*contentVector)[i]);
        if ((*netMap).count(routeVector[6]) > 0) {
            Route route;
            route.setNetName(routeVector[6]);
            route.setStartRowIndx(stoi(routeVector[0]));
            route.setStartColIndx(stoi(routeVector[1]));
            route.setStartLayIndx(stoi(routeVector[2]));
            route.setEndRowIndx(stoi(routeVector[3]));
            route.setEndColIndx(stoi(routeVector[4]));
            route.setEndlayIndx(stoi(routeVector[5]));
            vector<Route> routingVector = (*netMap)[routeVector[6]].getNumRoute();
            routingVector.push_back(route);
            (*netMap)[routeVector[6]].setNumRoute(routingVector);
            (*netNameSet).erase(routeVector[6]);
            reduceRoute(&(*gridVector), route.getStartLayIndx(), route.getEndlayIndx(), route.getStartRowIndx(),
                        route.getEndRowIndx(), route.getStartColIndx(), route.getEndColIndx(), &(*reducePointMap),
                        routeVector[6]);
        }
    }
    for (const auto netName : (*netNameSet)) {
        int layer = (*netMap)[netName].getConnectCell()[0].getLayerName();
        int row = (*netMap)[netName].getConnectCell()[0].getRowIndx();
        int col = (*netMap)[netName].getConnectCell()[0].getColIndx();
        (*gridVector)[layer - 1][row - 1][col - 1] -= 1;
    }
    *index = indexCount + numRoutes;
}

void ReadFile::getLayerGrid(GgridBoundaryIndex ggridBoundaryIndex, map<string, Layer> *layerMap,
                            vector<vector<vector<int> > > *gridVector) {
    int rowGridBegin = ggridBoundaryIndex.getRowBeginIdx();
    int colGridBegin = ggridBoundaryIndex.getColBeginIdx();
    int rowGridEnd = ggridBoundaryIndex.getRowEndIdx();
    int colGridEnd = ggridBoundaryIndex.getColEndIdx();
    int layerSize = (*layerMap).size();
    int ROWS = rowGridEnd + 1;
    int COLUMNS = colGridEnd + 1;
    int LAYER = layerSize + 1;

    for (int layer = 1; layer < LAYER; layer++) {
        vector<vector<int> > rowVector;
        for (int row = rowGridBegin; row < ROWS; row++) {
            vector<int> colVector;
            for (int col = colGridBegin; col < COLUMNS; col++) {
//                    string data = to_string(row) + "00" + to_string(col);
//                    int dataint = stoi(data);
//                    colVector.push_back(dataint);
                colVector.push_back((*layerMap)[to_string(layer)].getDefaultSupplyOfOneGrid());
            }
            rowVector.push_back(colVector);
        }
        (*gridVector).push_back(rowVector);
    }

    // blockage map<string, vector<string>> blockageCellMap
//        for (auto const &item : blockageCellMap) {
//            string masterCellName = item.first;
//            for (string cellName : item.second) {
//                int rowIndex = cellInstanceMap[cellName].getRowIndx();
//                int colIndex = cellInstanceMap[cellName].getColIndx();
//                map<string,Blockage> blockageType = masterCellMap[masterCellName].getBlockageType();
//                for(auto const &blockage : blockageType){
//                    int layer = blockage.second.getBlockageLayer();
//                    int demand = blockage.second.getDemand();
//                    gridVector[layer-1][rowIndex-1][colIndex-1] = gridVector[layer-1][rowIndex-1][colIndex-1] - demand;
//                }
//            }
//        }

}

//    vector<vector<vector<int> > > reduceRouteGridVector(vector<vector<vector<int> > > gridVector, map<string, Net> netMap){
//
//        for(auto const &item : netMap){
//            gridVector = reduceRouteSupply( gridVector,item.second.getNumRoute());
//        }
//
//        return gridVector;
//    }


//    vector<vector<vector<int> > > reduceRouteSupply(vector<vector<vector<int> > > gridVector,vector<Route> numRoute){
//
//        for (int i = 0; i<numRoute.size();i++) {
//          int startLayIndex =   numRoute[i].getStartLayIndx();
//          int endLayIndex = numRoute[i].getEndlayIndx();
//          int startRowIndex = numRoute[i].getStartRowIndx();
//          int endRowIndex = numRoute[i].getEndRowIndx();
//          int startColIndex = numRoute[i].getStartColIndx();
//          int endColIndex = numRoute[i].getEndColIndx();
//
//          reduceRoute( &gridVector, startLayIndex ,  endLayIndex, startRowIndex, endRowIndex, startColIndex, endColIndex);
//        }
//        return gridVector;
//    }
bool ReadFile::isReducePoint(string point, map<string, set<string>> *reducePointMap, string netName) {
    if ((*reducePointMap).count(netName) > 0) {
        int oriSize = (*reducePointMap)[netName].size();
        (*reducePointMap)[netName].insert(point);
        int afterSize = (*reducePointMap)[netName].size();
        if (oriSize != afterSize) {
            return false;
        } else {
            return true;
        }
    } else {
        set<string> pointSet;
        pointSet.insert(point);
        (*reducePointMap).insert(pair<string, set<string>>(netName, pointSet));
        return false;
    }

}

void
ReadFile::reduceRoute(vector<vector<vector<int> > > *gridVector, int startLayIndex, int endLayIndex, int startRowIndex,
                      int endRowIndex, int startColIndex, int endColIndex, map<string, set<string>> *reducePointMap,
                      string netName) {

    if (startLayIndex == endLayIndex and startColIndex == endColIndex) {
        if (endRowIndex > startRowIndex) {
            for (int rowIndex = startRowIndex; rowIndex <= endRowIndex; rowIndex++) {
                string point =
                        to_string(rowIndex) + "_" + to_string(startColIndex) + "_" + to_string(startLayIndex);
                if (isReducePoint(point, &(*reducePointMap), netName) == false) {
                    (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
                            (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] - 1;
                }
            }
        } else {
            for (int rowIndex = endRowIndex; rowIndex <= startRowIndex; rowIndex++) {
                string point =
                        to_string(rowIndex) + "_" + to_string(startColIndex) + "_" + to_string(startLayIndex);
                if (isReducePoint(point, &(*reducePointMap), netName) == false) {
                    (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
                            (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] - 1;
                }
            }
        }
    }
    if (startLayIndex == endLayIndex and startRowIndex == endRowIndex) {
        if (startColIndex < endColIndex) {
            for (int colIndex = startColIndex; colIndex <= endColIndex; colIndex++) {
                string point =
                        to_string(startRowIndex) + "_" + to_string(colIndex) + "_" + to_string(startLayIndex);
                if (isReducePoint(point, &(*reducePointMap), netName) == false) {
                    (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
                            (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] - 1;
                }
            }
        } else {
            for (int colIndex = endColIndex; colIndex <= startColIndex; colIndex++) {
                string point =
                        to_string(startRowIndex) + "_" + to_string(colIndex) + "_" + to_string(startLayIndex);
                if (isReducePoint(point, &(*reducePointMap), netName) == false) {
                    (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
                            (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] - 1;
                }
            }

        }
    }
    if (startLayIndex != endLayIndex) {
        string startPoint =
                to_string(startRowIndex) + "_" + to_string(startColIndex) + "_" + to_string(startLayIndex);
        string endPoint = to_string(endRowIndex) + "_" + to_string(endColIndex) + "_" + to_string(endLayIndex);
        if (isReducePoint(startPoint, &(*reducePointMap), netName) == false) {
            (*gridVector)[startLayIndex - 1][startRowIndex - 1][startColIndex - 1] =
                    (*gridVector)[startLayIndex - 1][startRowIndex - 1][startColIndex - 1] - 1;
        }
        if (isReducePoint(endPoint, &(*reducePointMap), netName) == false) {
            (*gridVector)[endLayIndex - 1][endRowIndex - 1][endColIndex - 1] =
                    (*gridVector)[endLayIndex - 1][endRowIndex - 1][endColIndex - 1] - 1;
        }
    }

}


void ReadFile::readVoltageArea(vector<string> *contentVector, map<string, VoltageArea> *voltageAreaMap, int *index) {
    int indexCount = (*index);
    VoltageArea voltageArea;
    vector<Grid> gridVector;
    vector<string> instanceVector;
    vector<string> voltage = splitString((*contentVector)[indexCount]);
    string areaName = voltage[1];
    voltageArea.setAreaName(voltage[1]);
    cout << "voltage Name : " << voltage[1] << endl;
    //GGrid data
    indexCount = indexCount + 1;
    vector<string> voltageGrid = splitString((*contentVector)[indexCount]);
    for (int i = indexCount + 1; i <= (indexCount + stoi(voltageGrid[1])); i++) {
        vector<string> gridvec = splitString((*contentVector)[i]);
        Grid grid;
        grid.setRowIndx(stoi(gridvec[0]));
        grid.setColIndx(stoi(gridvec[1]));
        gridVector.push_back(grid);
    }
    voltageArea.setGridVector(gridVector);
    //instance data
    indexCount = indexCount + stoi(voltageGrid[1]) + 1;

    vector<string> voltageInstance = splitString((*contentVector)[indexCount]);
    for (int i = indexCount + 1; i <= (indexCount + stoi(voltageInstance[1])); i++) {
//            vector<string> insvec = splitString(contentVector[i]);
        instanceVector.push_back((*contentVector)[i]);
    };
    voltageArea.setInstance(instanceVector);
    (*voltageAreaMap).insert(pair<string, VoltageArea>(areaName, voltageArea));
    indexCount = indexCount + stoi(voltageInstance[1]);
    *index = indexCount;
}


int ReadFile::getInsertIndex(vector<double> powerFactorVec, double number) {
    if (powerFactorVec.size() == 0) {
        return 0;
    }
    if (powerFactorVec.size() == 1) {
        if (powerFactorVec[0] > number) {
            return 1;
        } else {
            return 0;
        }
    }
    if (number <= powerFactorVec[powerFactorVec.size() - 1]) {
        return powerFactorVec.size();
    }
    int index = 0;
    for (int i = 0; i < powerFactorVec.size() - 1; i++) {
        if (number <= powerFactorVec[i] and number >= powerFactorVec[i + 1]) {
            index = i + 1;
            break;
        }
    }
    return index;
};


vector<string> ReadFile::splitByChar(string str, char pattern) {
    stringstream test(str);
    string segment;
    vector<string> seglist;
    while (std::getline(test, segment, pattern)) {
        seglist.push_back(segment);
    }
    return seglist;
}

//之後要改掉(此為Util 無法引入)
vector<string> ReadFile::splitString(string content) {
    int i = 0;
    vector<string> contentVector;
    stringstream contentArray(content);
    string word;
    while (contentArray.good()) {
        contentArray >> word;
        contentVector.push_back(word);
        ++i;
    }
    return contentVector;
}


//};

