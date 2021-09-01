//
// Created by Ｗilson on 2021/7/26.
//

#ifndef CELL_MOVEMENT_REROUTE_H
#define CELL_MOVEMENT_REROUTE_H

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

class ReRoute {
public:
    ReRoute();

    virtual ~ReRoute();

    void boundaryReroute(map<string, Net> *netMap,
                         map<string, CellInstance> *cellInstanceMap, map<string, MasterCell> *masterCellMap,
                         vector<vector<vector<int> > > *gridVector, map<string, vector<int > > *powerFactorMap,unordered_map<string,string> *isReRouteMap,double START);

    bool isOutOfBoundary(vector<Route> routeVec, map<string, int> boundaryMap);

    bool outOfBoundary(Route route, map<string, int> boundaryMap);

    bool isOverFlowHalfPerimeter(vector<Route> routeVec, map<string, int> boundaryMap);

    bool isRepeatVia(string coordinate, string upLayer, string downLayer, set<string> *viaSet);

    bool isRevise(string point, set<string> *routeSet);

    bool isViaSupplyValidFunction(int startLayer, int endLayer, int row, int col, vector<vector<vector<int > > > gridVector);

    void getSteinerPointRoute(Flute::Tree flutetree, vector<SteinerPoint> *steinerLine, vector<vector<vector<int > > > *gridVector,
                              map<string, vector<int > > *powerFactorMap, string minRoutingConstraint,
                              map<string, vector<SteinerPoint > > *layerSteinerVector, string reRoute);

    int caculatehalfPerimeter(map<string, int> boundaryMap);

    void getSteinerRoute(vector<Route> *routeVector, string reRouteNet, map<string, Net> *netMap,
                         vector<vector<vector<int > > > *gridVector, map<string, vector<int > > *powerFactorMap);

    void
    reviseRouteSupply(vector<vector<vector<int > > > *gridVector, vector<Route> *numRoute, string revise, string netName);

    void
    bottomLeftToTopRight(vector<SteinerPoint> *steinerLineVector, int startRowGrid, int endRowGrid, int startColGrid,
                         int endColGrid, vector<int> *layerPowerVectorV, vector<int> *layerPowerVectorH,
                         vector<vector<vector<int > > > *gridVector);

    void
    bottomRightToTopLeft(vector<SteinerPoint> *steinerLineVector, int startRowGrid, int endRowGrid, int startColGrid,
                         int endColGrid, vector<int> *layerPowerVectorV, vector<int> *layerPowerVectorH,
                         vector<vector<vector<int > > > *gridVector);

    void
    topLeftToBottomRight(vector <SteinerPoint> *steinerLineVector, int startRowGrid, int endRowGrid, int startColGrid,
                         int endColGrid, vector<int> *layerPowerVectorV, vector<int> *layerPowerVectorH,
                         vector<vector<vector<int > > > *gridVector);

    void
    topRightToBottomLeft(vector<SteinerPoint> *steinerLineVector, int startRowGrid, int endRowGrid, int startColGrid,
                         int endColGrid, vector<int> *layerPowerVectorV, vector<int> *layerPowerVectorH,
                         vector<vector<vector<int > > > *gridVector);
    bool checkDirection(vector<Route> *routeVector);
    int caculateRouteLength(vector<Route> *routeVector);
    void getSamePointReRoute(map<string,Net> *netMap, map<string, CellInstance> *cellInstanceMap, map<string, MasterCell> *masterCellMap,
                                      vector<vector<vector<int> > > *gridVector, map<string, vector<int> > *powerFactorMap,string reRouteNet);
};

#endif //CELL_MOVEMENT_REROUTE_H
