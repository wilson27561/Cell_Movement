//
// Created by Ｗilson on 2021/8/9.
//

#ifndef CELL_MOVEMENT_CELLMOVEROUTE_H
#define CELL_MOVEMENT_CELLMOVEROUTE_H
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <unordered_map>
#include "../Header/GgridBoundaryIndex.h"
#include "../Header/MasterCell.h"
#include "../Header/CellInstance.h"
#include "../Header/VoltageArea.h"
#include "../Header/Net.h"
#include "../flute/flute.h"
#include "../Header/SteinerPoint.h"
#include "../Util/ReRoute.h"
#include "Constant.h"

using namespace std;
class CellMoveRoute {
public:
    CellMoveRoute();

    virtual ~CellMoveRoute();

    void cellMoveReRoute(map<string, Net> *netMap,
                       map<string, CellInstance> *cellInstanceMap, vector<string> *emptyBlockageCellVector, map<string, MasterCell> *masterCellMap,
                       vector<vector<vector<int> > > *gridVector, map<string, vector<int > > *powerFactorMap,vector<CellInstance> *moveCellInstanceVector,int maxCellMovent, unordered_map<string,string> *isReRouteMap,unordered_map<string,VoltageArea> *voltageArearMap);
    void blockageReRoute(map<string, Net> *netMap,
                         map<string, CellInstance> *cellInstanceMap,vector<string> *emptyBlockageCellVector, map<string, MasterCell> *masterCellMap,
                         vector<vector<vector<int> > > *gridVector, map<string, vector<int > > *powerFactorMap,vector<CellInstance>  *moveCellInstanceVector,int maxCellMovent, unordered_map<string,string> *isReRouteMap, unordered_map<string,VoltageArea> *voltageArearMap);
    bool onOneSide(CellInstance cell,  map<string, CellInstance> *cellInstanceMap,map<string, Net> *netMap);
    void reRouteNet(string connectNet,map<string, Net> *netMap,map<string, CellInstance> *cellInstanceMap,map<string, vector<int> > *powerFactorMap,vector<vector<vector<int> > > *gridVector);
    void getCellMovePosition(string movedCell,set<string> *movedPositionVector,map<string, CellInstance> *cellInstanceMap,map<string, Net> *netMap);
    void getBoundaryMap(CellInstance movedCell,unordered_map<string,CellInstance> *connectCellMap,unordered_map<string,int> *boundaryMap);
    void getBoundaryBox(CellInstance cellInstanceVector,unordered_map<string,int> *boundaryMap);
    void movedPoint(unordered_map<string, int> *boundaryMap,vector<CellInstance> *movePosition,CellInstance oriCellInstance,map<string,MasterCell> *masterCellMap,vector<vector<vector<int> > > *gridVector,unordered_map<string,VoltageArea> *voltageArearMap);
    void reviseSupplyCellBlockage(MasterCell masterCell,vector<vector<vector<int> > > *gridVector,CellInstance cellInstance,string revise);
    bool isOverFlowDemand(map<string,Blockage> *blockageMap,vector<vector<vector<int> > > *gridVector,int rowIndx,int colIndx);
    bool isVoltageArea(int rowIndx,int colIndx,CellInstance cellInstance,unordered_map<string,VoltageArea> *voltageArearMap);
    int caculateRouteLength(vector<Route> *routeVector);
    void movedCellReRoute();
};


#endif //CELL_MOVEMENT_CELLMOVEROUTE_H
