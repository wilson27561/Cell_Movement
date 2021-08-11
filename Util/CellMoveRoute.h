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
                       vector<vector<vector<int> > > *gridVector, map<string, vector<int > > *powerFactorMap);
    void emptyBlockageReRoute(map<string, Net> *netMap,
                         map<string, CellInstance> *cellInstanceMap,vector<string> *emptyBlockageCellVector, map<string, MasterCell> *masterCellMap,
                         vector<vector<vector<int> > > *gridVector, map<string, vector<int > > *powerFactorMap);
    bool onOneSide(CellInstance cell,  map<string, CellInstance> *cellInstanceMap,map<string, Net> *netMap);

};


#endif //CELL_MOVEMENT_CELLMOVEROUTE_H
