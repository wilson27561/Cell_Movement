//
// Created by Ｗilson on 2021/5/15.
//

#include <iostream>
#include <vector>
#include <map>
#include "../Header/GgridBoundaryIndex.h"
#include "../Header/Layer.h"
#include "../Header/MasterCell.h"
#include "../Header/NumNonDefaultSupplyGgrid.h"
#include "../Header/CellInstance.h"
#include "../Header/VoltageArea.h"
#include "../Header/Net.h"
#include "Constant.h"

using namespace std;

class GgridBoundaryIndex;

class Util;

class ReadFile {
public :
    int readMaxCell(vector<string> lineVector) {
        return atoi(lineVector[1].c_str());
    }

    GgridBoundaryIndex readGGridBoundaryIdx(vector<string> lineVector) {
        GgridBoundaryIndex gridBoundaryIndex;
        gridBoundaryIndex.setRowBeginIdx(atoi(lineVector[1].c_str()));
        gridBoundaryIndex.setColBeginIdx(atoi(lineVector[2].c_str()));
        gridBoundaryIndex.setRowEndIdx(atoi(lineVector[3].c_str()));
        gridBoundaryIndex.setColEndIdx(atoi(lineVector[4].c_str()));
        return gridBoundaryIndex;
    }

    map<string, Layer> readLayer(vector<string> contentVector, int index, string layerCount) {
        map<string, Layer> layerMap;
        int mapSize = index + atoi(layerCount.c_str());
        for (int i = index + 1; i <= mapSize; i++) {
            vector<string> lineVector = splitString(contentVector[i]);
            Layer layer;
            layer.setLayerName(lineVector[1]);
            layer.setIndex(stoi(lineVector[2]));
            layer.setRoutingDirection(lineVector[3]);
            layer.setDefaultSupplyOfOneGrid(stoi(lineVector[4]));
            layer.setPowerFactor(stod(lineVector[5]));

            layerMap.insert(pair<string, Layer>(lineVector[2], layer));
        };
        return layerMap;
    }

    map<string, vector<int>> getLayerFacotr(map<string, Layer> layerMap, map<string, vector<int>> powerFactorMap) {
        vector<int> horizontalVector;
        vector<int> verticalVector;
        vector<double> H_PowerFactorVec;
        vector<double> V_PowerFactorVec;
        for (auto const &item : layerMap) {
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
        powerFactorMap.insert(pair<string, vector<int>>(HORIZONTAL, horizontalVector));
        powerFactorMap.insert(pair<string, vector<int>>(VERTICAL, verticalVector));

        return powerFactorMap;
    };
//    map<string, vector<NumNonDefaultSupplyGgrid>> readNumNonDefaultSupply(vector<string> contentVector,
//                                                                          map<string, vector<NumNonDefaultSupplyGgrid>> numNonDefaultSupplyMap,
//                                                                          int index, string layerCount) {
//        int mapSize = index + stoi(layerCount);
//        //  use vector to put NumNonDefaultSupplyGgrid
//        for (int i = index + 1; i <= mapSize; i++) {
//            vector<string> lineVector = splitString(contentVector[i]);
//            NumNonDefaultSupplyGgrid numNonDefaultSupply;
//            string layerName = LAYERPREFIX_M + lineVector[2];
//            numNonDefaultSupply.setRowIndx(stoi(lineVector[0]));
//            numNonDefaultSupply.setCollndx(stoi(lineVector[1]));
//            numNonDefaultSupply.setLayIndx(stoi(lineVector[2]));
//            numNonDefaultSupply.setIncrOrDecrValue(stoi(lineVector[3]));
//            numNonDefaultSupply.setLayerName(layerName);
//            if (numNonDefaultSupplyMap.count(layerName)) {
//                numNonDefaultSupplyMap[layerName].push_back(numNonDefaultSupply);
//            } else {
//                vector<NumNonDefaultSupplyGgrid> numNonDefaultSupplygridVector;
//                numNonDefaultSupplygridVector.push_back(numNonDefaultSupply);
//                numNonDefaultSupplyMap.insert(
//                        pair<string, vector<NumNonDefaultSupplyGgrid>>(layerName, numNonDefaultSupplygridVector));
//            }
//        }
//        return numNonDefaultSupplyMap;
//    }

    vector<NumNonDefaultSupplyGgrid>
    readNumNonDefaultSupply(vector<string> contentVector, vector<NumNonDefaultSupplyGgrid> numNonDefaultSupplyVector,
                            int index, string layerCount) {
        int mapSize = index + stoi(layerCount);
//  use vector to put NumNonDefaultSupplyGgrid
        for (int i = index + 1; i <= mapSize; i++) {
            vector<string> lineVector = splitString(contentVector[i]);
            NumNonDefaultSupplyGgrid numNonDefaultSupply;
            string layerName = LAYERPREFIX_M + lineVector[2];
            numNonDefaultSupply.setRowIndx(stoi(lineVector[0]));
            numNonDefaultSupply.setCollndx(stoi(lineVector[1]));
            numNonDefaultSupply.setLayIndx(stoi(lineVector[2]));
            numNonDefaultSupply.setIncrOrDecrValue(stoi(lineVector[3]));
            numNonDefaultSupply.setLayerName(layerName);
            numNonDefaultSupplyVector.push_back(numNonDefaultSupply);
        }
        return numNonDefaultSupplyVector;
    }

    map<string,vector<string>> readBlockageCell(vector<string> contentVector, vector<string> lineVector, map<string, vector<string>> blockageCellMap,
                                              int index){
        int blockageCount = stoi(lineVector[3]);
        string masterCellName = lineVector[1];
        if(blockageCount > 0){
            vector<string> blockageCellVector;
            blockageCellMap.insert(pair<string,vector<string>>(masterCellName,blockageCellVector));
        }
        return blockageCellMap;
    }



    map<string, MasterCell>
    readMasterCell(vector<string> contentVector, vector<string> lineVector, map<string, MasterCell> masterCellMap,
                   int index) {
        string masterCellName = lineVector[1];
        int pinCount = stoi(lineVector[2]);
        int blockageCount = stoi(lineVector[3]);

        MasterCell masterCell;
        masterCell.setMasterCellName(masterCellName);
        map<string, Pin> pinMap;
        map<string, Blockage> blockageMap;
        //PinCount
        for (int i = index + 1; i <= index + pinCount; i++) {
            vector<string> pinVector = splitString(contentVector[i]);
            Pin pin;
            pin.setPinName(pinVector[1]);
            pin.setLayer(pinVector[2]);
            pinMap.insert(pair<string, Pin>(pinVector[1], pin));
            masterCell.setPinType(pinMap);
        }
        //BlockageCount
        for (int i = index + pinCount + 1; i <= index + pinCount + blockageCount; i++) {
            vector<string> blockageVector = splitString(contentVector[i]);
            Blockage blockage;
            blockage.setBlockageName(blockageVector[1]);
            blockageVector[2].erase(std::remove(blockageVector[2].begin(), blockageVector[2].end(), 'M'), blockageVector[2].end());
            blockage.setBlockageLayer(stoi(blockageVector[2]));
            blockage.setDemand(stoi(blockageVector[3]));
            blockageMap.insert(pair<string, Blockage>(blockageVector[1], blockage));
            masterCell.setBlockageType(blockageMap);
        }
        masterCellMap.insert(pair<string, MasterCell>(masterCellName, masterCell));

        return masterCellMap;
    }

    map<string,vector<string>> readBlockageCell(vector<string> lineVector,map<string, vector<string>> blockageCellMap){
        string cellName = lineVector[1];
        string masterCellName =  lineVector[2];

        if(blockageCellMap.count(masterCellName) == true){
            blockageCellMap[masterCellName].push_back(cellName);
        }

        return blockageCellMap;
    }

    map<string, CellInstance> readCellInstance(vector<string> lineVector, map<string, CellInstance> cellInstanceMap) {
        CellInstance cellInstance;
        cellInstance.setCellName(lineVector[1]);
        //CellMasterName;
        cellInstance.setMasterCellName(lineVector[2]);
        cellInstance.setRowIndx(stoi(lineVector[3]));
        cellInstance.setColIndx(stoi(lineVector[4]));
        cellInstance.setMovalbe(lineVector[5]);
        cellInstanceMap.insert(pair<string, CellInstance>(lineVector[1], cellInstance));
        return cellInstanceMap;
    };


    map<string, int> readBoundary(vector<string> lineVector, map<string, int> boundaryMap) {
//        cout << " row : " << lineVector[3] << " col : "<<lineVector[4]  << endl;

        if (boundaryMap.find(UP) == boundaryMap.end()) {
            boundaryMap.insert(pair<string, int>(UP, stoi(lineVector[3])));
        } else {
            int oriUp = boundaryMap[UP];
            int newUp = stoi(lineVector[3]);
            if (newUp > oriUp) {
//                 boundaryMap.insert(pair<string, int>(UP,stoi(lineVector[3])));
                boundaryMap[UP] = stoi(lineVector[3]);

            }
        }
        if (boundaryMap.find(DOWN) == boundaryMap.end()) {
            boundaryMap.insert(pair<string, int>(DOWN, stoi(lineVector[3])));
        } else {
            int oriDown = boundaryMap[DOWN];
            int newDown = stoi(lineVector[3]);
            if (oriDown > newDown) {
//                boundaryMap.insert(pair<string, int>(DOWN,stoi(lineVector[3])));
                boundaryMap[DOWN] = stoi(lineVector[3]);
            }
        }
        //做邊
        if (boundaryMap.find(LEFT) == boundaryMap.end()) {
            boundaryMap.insert(pair<string, int>(LEFT, stoi(lineVector[4])));
        } else {
            int oriLeft = boundaryMap[LEFT];
            int newLeft = stoi(lineVector[4]);
            if (oriLeft > newLeft) {
                boundaryMap[LEFT] = stoi(lineVector[4]);
            }
        }
        if (boundaryMap.find(RIGHT) == boundaryMap.end()) {
            boundaryMap.insert(pair<string, int>(RIGHT, stoi(lineVector[4])));
        } else {
            int oriRight = boundaryMap[RIGHT];
            int newRight = stoi(lineVector[4]);
            if (newRight > oriRight) {
                boundaryMap[RIGHT] = stoi(lineVector[4]);
            }
        }
        return boundaryMap;
    }

    map<string, Net>
    readNet(vector<string> contentVector, vector<string> lineVector, map<string, Net> netMap,
            map<string, MasterCell> masterCellMap, map<string, CellInstance> cellInstanceMap, int index) {
        int pinCount = stoi(lineVector[2]);
        Net net;
        net.setNetName(lineVector[1]);
        net.setMinRoutingConstraint(lineVector[3]);
        net.setWeight(stod(lineVector[4]));
        vector<CellInstance> connectCellVector;

        for (int i = index + 1; i <= index + pinCount; i++) {

            vector<string> pinVector = splitString(contentVector[i]);
            vector<string> pinCellVec = splitByChar(pinVector[1], '/');
            //pinCellVec[0] -> C1 pinCellVec[1] -> P1 一個net可能會連接同一個cell 不同  pin
            string masterCellName = cellInstanceMap[pinCellVec[0]].getMasterCellName();
            map<string, Pin> pinMap = masterCellMap[masterCellName].getPinType();
            CellInstance cellinstance;
            cellinstance.setCellName(pinCellVec[0]);
            cellinstance.setConnectPin(pinCellVec[1]);
//            cout << "cell Name : " << pinCellVec[0] <<" Pin :" << pinCellVec[1] << " layer : "  <<pinMap[pinCellVec[1]].getLayer()<< endl;
            string layer = pinMap[pinCellVec[1]].getLayer();
            layer.erase(std::remove(layer.begin(), layer.end(), 'M'), layer.end());
            cellinstance.setLayerName(stoi(layer));
            cellinstance.setRowIndx(cellInstanceMap[pinCellVec[0]].getRowIndx());
            cellinstance.setColIndx(cellInstanceMap[pinCellVec[0]].getColIndx());
            connectCellVector.push_back(cellinstance);
        }
        net.setConnectCell(connectCellVector);
        netMap.insert(pair<string, Net>(lineVector[1], net));

        return netMap;
    }

    map<string, Net>
    readRoute(vector<string> contentVector, vector<string> lineVector, map<string, Net> netMap, int index) {
        int numRoutes = stoi(lineVector[1]);
        for (int i = index + 1; i <= index + numRoutes; i++) {
            vector<string> routeVector = splitString(contentVector[i]);
            if (netMap.count(routeVector[6])) {
                Route route;
                route.setNetName(routeVector[6]);
                route.setStartRowIndx(stoi(routeVector[0]));
                route.setStartColIndx(stoi(routeVector[1]));
                route.setStartLayIndx(stoi(routeVector[2]));
                route.setEndRowIndx(stoi(routeVector[3]));
                route.setEndColIndx(stoi(routeVector[4]));
                route.setEndlayIndx(stoi(routeVector[5]));
                vector<Route> routingVector = netMap[routeVector[6]].getNumRoute();
                routingVector.push_back(route);
//                cout <<"vec size : " << routeVector[6]<< endl;
                netMap[routeVector[6]].setNumRoute(routingVector);
//               cout <<"size : " << netMap[routeVector[6]].getNumRoute().size()<< endl;
//               netMap[routeVector[6]].getNumRoute().push_back(route);
            }
        }
        return netMap;
    }

    vector<vector<vector<int> > > getLayerGrid(GgridBoundaryIndex ggridBoundaryIndex, map<string, Layer> layerMap,
                                               vector<vector<vector<int> > > gridVector,
                                               vector<NumNonDefaultSupplyGgrid> numNonDefaultSupplyVector, map<string, vector<string>> blockageCellMap,map<string, CellInstance> cellInstanceMap,map<string, MasterCell> masterCellMap) {
        int rowGridBegin = ggridBoundaryIndex.getRowBeginIdx();
        int colGridBegin = ggridBoundaryIndex.getColBeginIdx();
        int rowGridEnd = ggridBoundaryIndex.getRowEndIdx();
        int colGridEnd = ggridBoundaryIndex.getColEndIdx();
        int layerSize = layerMap.size();
        int ROWS = rowGridEnd + 1;
        int COLUMNS = colGridEnd + 1;
        int LAYER = layerSize + 1;
//        cout << "ROWS : " << ROWS << endl;
//        cout << "COLUMNS : " << COLUMNS << endl;
//        cout << "LAYER : " << LAYER << endl;

        for (int layer = 1; layer < LAYER; layer++) {
            vector<vector<int> > rowVector;
            for (int row = rowGridBegin; row < ROWS; row++) {
                vector<int> colVector;
                for (int col = colGridBegin; col < COLUMNS; col++) {
//                    string data = to_string(layer) + to_string(row) + to_string(col);
//                    int dataint = stoi(data);
//                    colVector.push_back(dataint);
                    colVector.push_back(layerMap[to_string(layer)].getDefaultSupplyOfOneGrid());
                }
                rowVector.push_back(colVector);
            }
            gridVector.push_back(rowVector);
        }
        // non - default supply grid
//        for (auto const &numNonDefaultSupply : numNonDefaultSupplyVector) {
//            gridVector[numNonDefaultSupply.getLayIndx() - 1][numNonDefaultSupply.getRowIndx() - 1][numNonDefaultSupply.getCollndx() - 1] =  gridVector[numNonDefaultSupply.getLayIndx() - 1][numNonDefaultSupply.getRowIndx() - 1][numNonDefaultSupply.getCollndx() - 1] + numNonDefaultSupply.getIncrOrDecrValue();
//        }
//        cout << "blockage Type : " << endl;
        // blockage
        for (auto const &item : blockageCellMap) {
            string masterCellName = item.first;
            for (string cellName : item.second) {
                int rowIndex = cellInstanceMap[cellName].getRowIndx();
                int colIndex = cellInstanceMap[cellName].getColIndx();
                map<string,Blockage> blockageType = masterCellMap[masterCellName].getBlockageType();
                for(auto const &blockage : blockageType){
                    int layer = blockage.second.getBlockageLayer();
                    int demand = blockage.second.getDemand();
                    gridVector[layer-1][rowIndex-1][colIndex-1] = gridVector[layer-1][rowIndex-1][colIndex-1] - demand;
                }
            }
        }


        //實際位置開頭由1開始
//        for (int layer = 0; layer < layerSize; layer++) {
//            for (int row = rowGridEnd - 1; row >= 0; row--) {
//                for (int col = 0; col < colGridEnd; col++) {
//                    std::cout << gridVector[layer][row][col] << "\t";
//                }
//                std::cout << "" << std::endl;
//            }
//            std::cout << "" << std::endl;
//        }

        return gridVector;
    }

    vector<vector<vector<int> > > reduceRouteGridVector(vector<vector<vector<int> > > gridVector, map<string, Net> netMap){

        for(auto const &item : netMap){
            gridVector = reduceRouteSupply( gridVector,item.second.getNumRoute());
        }

        return gridVector;
    }


    vector<vector<vector<int> > > reduceRouteSupply(vector<vector<vector<int> > > gridVector,vector<Route> numRoute){

        for (int i = 0; i<numRoute.size();i++) {
          int startLayIndex =   numRoute[i].getStartLayIndx();
          int endLayIndex = numRoute[i].getEndlayIndx();
          int startRowIndex = numRoute[i].getStartRowIndx();
          int endRowIndex = numRoute[i].getEndRowIndx();
          int startColIndex = numRoute[i].getStartColIndx();
          int endColIndex = numRoute[i].getEndColIndx();
//          cout <<  startRowIndex << " " << startColIndex << " " << startLayIndex << " " << endRowIndex << " " << endColIndex << " "<< endLayIndex << " " <<  numRoute[i].getNetName()<<endl;
          if(startLayIndex == endLayIndex and startColIndex == endColIndex){
              if(startRowIndex > endRowIndex){
                  for (int rowIndex = startRowIndex; rowIndex <=endRowIndex ; rowIndex++) {
                      gridVector[startLayIndex-1][rowIndex-1][startColIndex-1] = gridVector[startLayIndex-1][rowIndex-1][startColIndex-1] -1;
                  }
              }else{
                  for (int rowIndex = endRowIndex; rowIndex >=startRowIndex ; rowIndex--) {
                      gridVector[startLayIndex-1][rowIndex-1][startColIndex-1] = gridVector[startLayIndex-1][rowIndex-1][startColIndex-1] -1;
                  }
              }
          }
          if(startLayIndex == endLayIndex and startRowIndex == endRowIndex){
              for (int colIndex = startColIndex; colIndex <=endColIndex ; colIndex++) {
                  gridVector[startLayIndex-1][startRowIndex-1][colIndex-1] =  gridVector[startLayIndex-1][startRowIndex-1][colIndex-1] -1;
              }
          }else{
              for (int colIndex = endColIndex; colIndex >=startColIndex ; colIndex--) {
                  gridVector[startLayIndex-1][startRowIndex-1][colIndex-1] =  gridVector[startLayIndex-1][startRowIndex-1][colIndex-1] -1;
              }
          }
        }
        return gridVector;
    }


    map<string, VoltageArea>
    readVoltageArea(vector<string> contentVector, vector<string> lineVector, map<string, VoltageArea> voltageAreaMap,
                    int index) {
        VoltageArea voltageArea;
        vector<Grid> gridVector;
        vector<string> instanceVector;
        for (int i = index + 1; i < contentVector.size(); i++) {
            vector<string> voltage = splitString(contentVector[i]);
            if (voltage[0] == NAME) {
                voltageArea.setAreaName(voltage[1]);
            } else if (voltage[0] == GGRIDS) {
                int gridCount = stoi(voltage[1]);
                for (int j = i + 1; j <= i + gridCount; j++) {
                    vector<string> voltage = splitString(contentVector[j]);
                    Grid grid;
                    grid.setRowIndx(stoi(voltage[0]));
                    grid.setColIndx(stoi(voltage[1]));
                    gridVector.push_back(grid);
                }
                voltageArea.setGridVector(gridVector);
                i = i + gridCount;
            } else if (voltage[0] == INSTANCES) {
                int instanceCount = stoi(voltage[1]);
                for (int j = i + 1; j <= i + instanceCount; j++) {
                    vector<string> voltage = splitString(contentVector[j]);
                    instanceVector.push_back(voltage[0]);
                }
                voltageArea.setInstance(instanceVector);
                i = i + instanceCount;
            }
        }
        return voltageAreaMap;
    }

    int getInsertIndex(vector<double> powerFactorVec, double number){
        if(powerFactorVec.size() == 0){
            return 0;
        }
        if(powerFactorVec.size() == 1){
            if (powerFactorVec[0]>number)
            {
                return 1;
            }else
            {
                return 0;
            }
        }
        if(number <= powerFactorVec[powerFactorVec.size()-1]){
            return powerFactorVec.size();
        }
        int index = 0;
        for (int i = 0; i < powerFactorVec.size()-1; i++)
        {
            if(number <= powerFactorVec[i] and number>= powerFactorVec[i+1]){
                index = i+1;
                break;
            }
        }
        return index;
    };


    vector<string> splitByChar(string str, char pattern) {
        stringstream test(str);
        string segment;
        vector<string> seglist;
        while (std::getline(test, segment, pattern)) {
            seglist.push_back(segment);
        }
        return seglist;
    }

//之後要改掉(此為Util 無法引入)
    vector<string> splitString(string content) {
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


};

