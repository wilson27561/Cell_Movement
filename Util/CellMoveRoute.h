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
                       vector<vector<vector<int> > > *gridVector, map<string, vector<int > > *powerFactorMap,vector<CellInstance> *moveCellInstanceVector,int maxCellMovent, unordered_map<string,string> *isReRouteMap);
    void emptyBlockageReRoute(map<string, Net> *netMap,
                         map<string, CellInstance> *cellInstanceMap,vector<string> *emptyBlockageCellVector, map<string, MasterCell> *masterCellMap,
                         vector<vector<vector<int> > > *gridVector, map<string, vector<int > > *powerFactorMap,vector<CellInstance>  *moveCellInstanceVector,int maxCellMovent, unordered_map<string,string> *isReRouteMap);
    bool onOneSide(CellInstance cell,  map<string, CellInstance> *cellInstanceMap,map<string, Net> *netMap);
    void getRoutePointVector(string reRouteNet,map<string, Net> *netMap,map<string, CellInstance> *cellInstanceMap,vector<RoutePoint> *routePointVector);
    void reRouteNet(string connectNet,map<string, Net> *netMap,map<string, CellInstance> *cellInstanceMap,map<string, vector<int> > *powerFactorMap,vector<vector<vector<int> > > *gridVector);
    void getCellMovePosition(string movedCell,set<string> *movedPositionVector,map<string, CellInstance> *cellInstanceMap,map<string, Net> *netMap);
    void getRandomCellVector(set<string> *cellMovePositionSet,vector<string> *cellMovePositionVector);
    void getBoundaryMap(CellInstance movedCell,unordered_map<string,CellInstance> *connectCellMap,unordered_map<string,int> *boundaryMap);
    void getBoundaryBox(CellInstance cellInstanceVector,unordered_map<string,int> *boundaryMap);
    void randomMovingPoint(unordered_map<string,int> boundaryMap,CellInstance *moveCellInstance,string cellName);
    bool isRepeatCellMove(CellInstance moveCellInstance,set<string> *cellMoveSet);

};


#endif //CELL_MOVEMENT_CELLMOVEROUTE_H
