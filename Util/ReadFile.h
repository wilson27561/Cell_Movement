//
// Created by Ｗilson on 2021/7/26.
//

#ifndef CELL_MOVEMENT_READFILE_H
#define CELL_MOVEMENT_READFILE_H


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
class ReadFile {
public:

    ReadFile();

    virtual ~ReadFile();

    int readMaxCell(vector<string> lineVector);

    GgridBoundaryIndex readGGridBoundaryIdx(vector<string> lineVector);

    void readLayer(vector<string> *contentVector, int *index, string layerCount, map<string, Layer> *layerMap);

    void getLayerFacotr(map<string, Layer> *layerMap, map<string, vector<int > > *powerFactorMap);

    void readNumNonDefaultSupply(vector<string> *contentVector, vector<vector<vector<int > > > *gridVector, int *index,
                                 string layerCount);

    void
    readMasterCell(vector<string> *contentVector, vector<string> *lineVector, map<string, MasterCell> *masterCellMap,
                   int *index);

    void readCellInstance(vector<string> lineVector, map<string, CellInstance> *cellInstanceMap, vector<string> *emptyBlockageCellVector,
                          map<string, MasterCell> *masterCellMap, map<string, map<string, Blockage > > *blockageCellMap,
                          vector<vector<vector<int > > > *gridVector);

    void readBoundary(CellInstance cellInstance, map<string, int> *boundaryMap);

    void readNet(vector<string> *contentVector, vector<string> *lineVector, map<string, Net> *netMap,
                 map<string, MasterCell> *masterCellMap, map<string, CellInstance> *cellInstanceMap,
                 set<string> *netNameSet, int *index);

    void readRoute(vector<string> *contentVector, vector<string> *lineVector, map<string, Net> *netMap,
                   vector<vector<vector<int > > > *gridVector, int *index, map<string, set<string > > *reducePointMap,
                   set<string> *netNameSet);

    vector<string> splitByChar(string str, char pattern);

    vector<string> splitString(string content);

    int getInsertIndex(vector<double> powerFactorVec, double number);

    void getLayerGrid(GgridBoundaryIndex ggridBoundaryIndex, map<string, Layer> *layerMap,
                      vector<vector<vector<int > > > *gridVector);

    void reduceRoute(vector<vector<vector<int > > > *gridVector, int startLayIndex, int endLayIndex, int startRowIndex,
                     int endRowIndex, int startColIndex, int endColIndex, map<string, set<string > > *reducePointMap,
                     string netName);

    bool isReducePoint(string point, map<string, set<string > > *reducePointMap, string netName);

    void readVoltageArea(vector<string> *contentVector, unordered_map<string, VoltageArea> *voltageAreaMap, int *index);
};

#endif //CELL_MOVEMENT_READFILE_H
