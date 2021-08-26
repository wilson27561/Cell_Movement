//
// Created by Ｗilson on 2021/5/30.
//

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "../Header/GgridBoundaryIndex.h"
#include "../Header/MasterCell.h"
#include "../Header/CellInstance.h"
#include "../Header/VoltageArea.h"
#include "../Header/Net.h"
#include "../flute/flute.h"
#include "../Header/SteinerPoint.h"
#include "../Util/ReRoute.h"
#include "../Header/RoutePoint.h"
#include "Constant.h"

using namespace std;

class GgridBoundaryIndex;

class Util;

class flute;

ReRoute::ReRoute() {}

ReRoute::~ReRoute() {
}


void ReRoute::boundaryReroute(map<string, Net> *netMap,
                              map<string, CellInstance> *cellInstanceMap, map<string, MasterCell> *masterCellMap,
                              vector<vector<vector<int> > > *gridVector, map<string, vector<int> > *powerFactorMap,double START,unordered_map<string,string> *isReRouteMap) {

    double STARTROUTE, ENDROUTE;
    STARTROUTE = clock();
    //TODO 先檢查完需要做的reroute，再依net的weight順序做排序
    //TODO 拔一條繞一條   ok
    //TODO 確認routingLayer按比重   ok
    //TODO 確認minimumRoutingConstraint  ok (確認是否要從 1開始繞，還是可以從最minimumconstraint那一層開始去做繞線)
    //TODO 超過半週長要重繞   ok
    //TODO two pin 做 cell move
    //TODO 多執行緒
    //TODO 是否要將via 放到兩條線中間
    for (const auto &item : (*netMap)) {
        vector<Route> routeVec = item.second.getNumRoute();
        bool isNeedReroute = false;
        //判斷net 是否 需要重繞
        if (isOutOfBoundary(routeVec, item.second.getBoundaryMap())) {
            isNeedReroute = true;
        } else if (isOverFlowHalfPerimeter(routeVec, item.second.getBoundaryMap()) ) {
            isNeedReroute = true;
        } else {
            isNeedReroute = false;
        };

        //-------  check bounding route start -------
        if (isNeedReroute) {
            cout << "Need Reroute Name : " << item.first << endl;
            //拔掉線段 supply add
            vector<Route> numRoute = item.second.getNumRoute();
            //加上原來的線段grid
            reviseRouteSupply(&(*gridVector), &numRoute, ADD, item.first);

            vector<Route> routeVector;
            getSteinerRoute(&routeVector, item.first, &(*netMap), &(*gridVector), &(*powerFactorMap));
            if (routeVector.size() > 0) {
                (*netMap)[item.first].setNumRoute(routeVector);
                //減掉新的線段
                reviseRouteSupply(&(*gridVector), &routeVector, REDUCE, item.first);
            } else {
                //減掉原來的線段
                reviseRouteSupply(&(*gridVector), &numRoute, REDUCE, item.first);
            }
        }
        //-------  check bounding route end -------
        ENDROUTE = clock();
        if((ENDROUTE - STARTROUTE) / CLOCKS_PER_SEC >= 2700){
            break;
        }
    }
}

void ReRoute::reviseRouteSupply(vector<vector<vector<int> > > *gridVector, vector<Route> *numRoute, string revise,
                                string netName) {
    set<string> routeSet;
    for (int i = 0; i < (*numRoute).size(); i++) {
        int startLayIndex = (*numRoute)[i].getStartLayIndx();
        int endLayIndex = (*numRoute)[i].getEndlayIndx();
        int startRowIndex = (*numRoute)[i].getStartRowIndx();
        int endRowIndex = (*numRoute)[i].getEndRowIndx();
        int startColIndex = (*numRoute)[i].getStartColIndx();
        int endColIndex = (*numRoute)[i].getEndColIndx();
        if (startLayIndex == endLayIndex and startColIndex == endColIndex) {
            if (startRowIndex < endRowIndex) {
                for (int rowIndex = startRowIndex; rowIndex <= endRowIndex; rowIndex++) {
                    string point =
                            to_string(rowIndex) + "_" + to_string(startColIndex) + "_" + to_string(startLayIndex);
                    if (revise == REDUCE) {
                        if (isRevise(point, &routeSet) == false) {
                            (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
                                    (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] - 1;
                        }
                    } else {
                        if (isRevise(point, &routeSet) == false) {
                            (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
                                    (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] + 1;
                        }
                    }

                }
            } else {
                for (int rowIndex = endRowIndex; rowIndex <= startRowIndex; rowIndex++) {
                    string point =
                            to_string(rowIndex) + "_" + to_string(startColIndex) + "_" + to_string(startLayIndex);
                    if (revise == REDUCE) {
                        if (isRevise(point, &routeSet) == false) {
                            (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
                                    (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] - 1;
                        }
                    } else {
                        if (isRevise(point, &routeSet) == false) {
                            (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
                                    (*gridVector)[startLayIndex - 1][rowIndex - 1][startColIndex - 1] + 1;
                        }
                    }
                }
            }
        }
        if (startLayIndex == endLayIndex and startRowIndex == endRowIndex) {
            if (startColIndex < endColIndex) {
                for (int colIndex = startColIndex; colIndex <= endColIndex; colIndex++) {
                    string point =
                            to_string(startRowIndex) + "_" + to_string(colIndex) + "_" + to_string(startLayIndex);
                    if (revise == REDUCE) {
                        if (isRevise(point, &routeSet) == false) {
                            (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
                                    (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] - 1;
                        }
                    } else {
                        if (isRevise(point, &routeSet) == false) {
                            (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
                                    (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] + 1;
                        }

                    }
                }
            } else {
                for (int colIndex = endColIndex; colIndex <= startColIndex; colIndex++) {
                    string point =
                            to_string(startRowIndex) + "_" + to_string(colIndex) + "_" + to_string(startLayIndex);
                    if (revise == REDUCE) {
                        if (isRevise(point, &routeSet) == false) {
                            (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
                                    (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] - 1;
                        }
                    } else {
                        if (isRevise(point, &routeSet) == false) {
                            (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
                                    (*gridVector)[startLayIndex - 1][startRowIndex - 1][colIndex - 1] + 1;
                        }
                    }
                }
            }
        }
        if (startLayIndex != endLayIndex) {
            if (startLayIndex < endLayIndex) {
                for (int layIndex = startLayIndex; layIndex <= endLayIndex; layIndex++) {
                    string point =
                            to_string(startRowIndex) + "_" + to_string(startColIndex) + "_" + to_string(layIndex);
                    if (isRevise(point, &routeSet) == false) {
                        if (revise == REDUCE) {
                            (*gridVector)[layIndex - 1][startRowIndex - 1][startColIndex - 1] =
                                    (*gridVector)[layIndex - 1][startRowIndex - 1][startColIndex - 1] - 1;
                        } else {
                            (*gridVector)[layIndex - 1][startRowIndex - 1][startColIndex - 1] =
                                    (*gridVector)[layIndex - 1][startRowIndex - 1][startColIndex - 1] + 1;
                        }
                    }
                }
            } else {
                for (int layIndex = endLayIndex; layIndex <= startLayIndex; layIndex++) {
                    string point =
                            to_string(startRowIndex) + "_" + to_string(startColIndex) + "_" + to_string(layIndex);
                    if (isRevise(point, &routeSet) == false) {
                        if (revise == REDUCE) {
                            (*gridVector)[layIndex - 1][startRowIndex - 1][startColIndex - 1] =
                                    (*gridVector)[layIndex - 1][startRowIndex - 1][startColIndex - 1] - 1;
                        } else {
                            (*gridVector)[layIndex - 1][startRowIndex - 1][startColIndex - 1] =
                                    (*gridVector)[layIndex - 1][startRowIndex - 1][startColIndex - 1] + 1;
                        }
                    }
                }
            }
        }


    }
}

void ReRoute::transferFluteTree(vector<RoutePoint> *routePointVector,Flute::Tree flutetree){
    for (int i = 0; i < 2 * flutetree.deg - 2; i++) {
        RoutePoint routePoint;
        int steinerPointRow = flutetree.branch[flutetree.branch[i].n].x;
        int steinerPointCol = flutetree.branch[flutetree.branch[i].n].y;
        int cellPointRow = flutetree.branch[i].x;
        int cellPointCol = flutetree.branch[i].y;
        routePoint.setStartPointRow(flutetree.branch[flutetree.branch[i].n].x);
        routePoint.setEndPointCol(flutetree.branch[flutetree.branch[i].n].y);
        routePoint.setEndPointRow(flutetree.branch[i].x);
        routePoint.setEndPointCol(flutetree.branch[i].y);
        (*routePointVector).push_back(routePoint);
    }
}




//public:
//取得Steiner point cellpoint 為起點 steinerPoint 為終點
void ReRoute::getSteinerPointRoute(vector<RoutePoint> *routePointVector, vector<SteinerPoint> *steinerLine,
                                   vector<vector<vector<int> > > *gridVector,
                                   map<string, vector<int> > *powerFactorMap, string minRoutingConstraint,
                                   map<string, vector<SteinerPoint> > *layerSteinerVector, string reRoute) {

    vector<int> layerPowerVectorH;
    vector<int> layerPowerVectorV;

    //-------  miniRoutingConstraint  start -------
    if (minRoutingConstraint != "NoCstr") {
        minRoutingConstraint.erase(std::remove(minRoutingConstraint.begin(), minRoutingConstraint.end(), 'M'),
                                   minRoutingConstraint.end());
        int constraint = stoi(minRoutingConstraint);
        for (int i = 0; i < (*powerFactorMap)[HORIZONTAL].size(); i++) {
            if ((*powerFactorMap)[HORIZONTAL][i] >= constraint) {
                layerPowerVectorH.push_back((*powerFactorMap)[HORIZONTAL][i]);
            }
        }
        for (int i = 0; i < (*powerFactorMap)[VERTICAL].size(); i++) {
            if ((*powerFactorMap)[VERTICAL][i] >= constraint) {
                layerPowerVectorV.push_back((*powerFactorMap)[VERTICAL][i]);
            }
        }
    } else {
        layerPowerVectorH = (*powerFactorMap)[HORIZONTAL];
        layerPowerVectorV = (*powerFactorMap)[VERTICAL];
    }
    //-------  miniRoutingConstraint  end -------


    //-------  routing by steiner point  start -------
    bool isValidRoute = true;

//    for (int i = 0; i < 2 * flutetree.deg - 2; i++) {
        for(int i =0;i< (*routePointVector).size();i++){
//        int steinerPointRow = flutetree.branch[flutetree.branch[i].n].x;
//        int steinerPointCol = flutetree.branch[flutetree.branch[i].n].y;
//        int cellPointRow = flutetree.branch[i].x;
//        int cellPointCol = flutetree.branch[i].y;

        int steinerPointRow = (*routePointVector)[i].getStartPointRow();
        int steinerPointCol = (*routePointVector)[i].getStartPointCol();
        int cellPointRow = (*routePointVector)[i].getEndPointRow();
        int cellPointCol = (*routePointVector)[i].getEndPointCol();


        int routeLayer = 0;
//            cout << "steiner point : " << cellPointRow << " " << cellPointCol  << " " << steinerPointRow << " " << steinerPointCol  << endl;
        if (steinerPointRow == cellPointRow and steinerPointCol != cellPointCol) {
            int rowGrid = steinerPointRow - 1;
            int startColGrid = cellPointCol - 1;
            int endColGrid = steinerPointCol - 1;
            if (startColGrid < endColGrid) {
                for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                    bool canRoute = true;
                    for (int col = startColGrid; col <= endColGrid; col++) {
//                            cout << gridVector[(layerPowerVectorH[layer] - 1)][rowGrid][col] << " ";
                        if ((*gridVector)[(layerPowerVectorH[layer] - 1)][rowGrid][col] <= 0) {
                            canRoute = false;
                            isValidRoute = false;
                        }
                    }
                    if (canRoute) {
                        routeLayer = layerPowerVectorH[layer];
                        SteinerPoint steinerPoint(steinerPointRow, steinerPointCol, cellPointRow,
                                                  cellPointCol, routeLayer);
//                           steinerMap.insert(pair<string, SteinerPoint>(coordinate, steinerPoint));
                        (*steinerLine).push_back(steinerPoint);
                        break;
                    }
                }

            } else {
                for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                    bool canRoute = true;
                    for (int col = startColGrid; col >= endColGrid; col--) {
//                            std::cout << gridVector[(layerPowerVectorH[layer] - 1)][rowGrid][col] << " ";
                        if ((*gridVector)[(layerPowerVectorH[layer] - 1)][rowGrid][col] <= 0) {
                            canRoute = false;
                            isValidRoute = false;
                        }
                    }
                    if (canRoute) {
                        routeLayer = layerPowerVectorH[layer];
                        SteinerPoint steinerPoint(steinerPointRow, steinerPointCol, cellPointRow,
                                                  cellPointCol, routeLayer);
//                            steinerMap.insert(pair<string, SteinerPoint>(coordinate, steinerPoint));
                        (*steinerLine).push_back(steinerPoint);
                        break;
                    }
                }
//                    cout << "" << endl;
            }
            // 如果都不能繞線，要在這裡面做give up route
        } else if (steinerPointCol == cellPointCol and steinerPointRow != cellPointRow) {
            int colGrid = cellPointCol - 1;
            int startRowGrid = cellPointRow - 1;
            int endRowGrid = steinerPointRow - 1;
            if (startRowGrid < endRowGrid) {
                bool canRoute = true;
                for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                    for (int row = startRowGrid; row <= endRowGrid; row++) {
                        if ((*gridVector)[(layerPowerVectorV[layer] - 1)][row][colGrid] <= 0) {
                            canRoute = false;
                            isValidRoute = false;
                        }
                    }
                    if (canRoute) {
                        routeLayer = layerPowerVectorV[layer];
                        SteinerPoint steinerPoint(steinerPointRow, steinerPointCol, cellPointRow,
                                                  cellPointCol, routeLayer);
//                            steinerMap.insert(pair<string, SteinerPoint>(coordinate, steinerPoint));
                        (*steinerLine).push_back(steinerPoint);
                        break;
                    }
                }
            } else {
                for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                    bool canRoute = true;
                    for (int row = startRowGrid; row >= endRowGrid; row--) {
//                            std::cout << gridVector[(layerPowerVectorV[layer] - 1)][row][colGrid] << " ";
                        if ((*gridVector)[(layerPowerVectorV[layer] - 1)][row][colGrid] <= 0) {
                            canRoute = false;
                            isValidRoute = false;
                        }
                    }
                    if (canRoute) {
                        routeLayer = layerPowerVectorV[layer];
                        SteinerPoint steinerPoint(steinerPointRow, steinerPointCol, cellPointRow,
                                                  cellPointCol, routeLayer);
//                            steinerMap.insert(pair<string, SteinerPoint>(coordinate, steinerPoint));
                        (*steinerLine).push_back(steinerPoint);
                        break;
                    }
                }
//                    cout << "" << endl;
            }
        } else if (steinerPointCol == cellPointCol and steinerPointRow == cellPointRow) {
            //之後可以使用pointMap 這個點去做
//                cout << "cellPointCol and steinerPointCol is on same Point" << endl;
        } else {
            // Z-pattern route L-pattern route
//                cout << "get in side z pattern " << endl;
            int startRowGrid = cellPointRow - 1;
            int startColGrid = cellPointCol - 1;
            int endRowGrid = steinerPointRow - 1;
            int endColGrid = steinerPointCol - 1;
//                cout << "cell : " << cellPointRow << " " << cellPointCol << " steinerPoint : " << steinerPointRow << " "
//                     << steinerPointCol << endl;
//                cout << " Cell to SteinerPoint" << endl;
            if (steinerPointRow > cellPointRow and steinerPointCol > cellPointCol) {
//                    cout << "bottomLeftToTopRight" << endl;
                //左下到右上
                int oriSize = (*steinerLine).size();
                bottomLeftToTopRight(&(*steinerLine), startRowGrid, endRowGrid, startColGrid,
                                     endColGrid, &layerPowerVectorV, &layerPowerVectorH,
                                     &(*gridVector));
                int afterSize = (*steinerLine).size();
                if (oriSize == afterSize) {
                    isValidRoute = false;
                }
            } else if (steinerPointRow > cellPointRow and steinerPointCol < cellPointCol) {
//                    cout << "bottomRightToTopLeft" << endl;
                //右下到左上
                int oriSize = (*steinerLine).size();
                bottomRightToTopLeft(&(*steinerLine), startRowGrid, endRowGrid, startColGrid,
                                     endColGrid, &layerPowerVectorV, &layerPowerVectorH,
                                     &(*gridVector));
                int afterSize = (*steinerLine).size();
                if (oriSize == afterSize) {
                    isValidRoute = false;
                }
            } else if (steinerPointRow < cellPointRow and steinerPointCol > cellPointCol) {
//                    cout << "topLeftToBottomRight" << endl;
                //左上到右下
                int oriSize = (*steinerLine).size();
                topLeftToBottomRight(&(*steinerLine), startRowGrid, endRowGrid, startColGrid,
                                     endColGrid, &layerPowerVectorV, &layerPowerVectorH,
                                     &(*gridVector));
                int afterSize = (*steinerLine).size();
                if (oriSize == afterSize) {
                    isValidRoute = false;
                }

            } else {
//                    cout << "topRightToBottomLeft" << endl;
                //右上到左下
                int oriSize = (*steinerLine).size();
                topRightToBottomLeft(&(*steinerLine), startRowGrid, endRowGrid, startColGrid,
                                     endColGrid, &layerPowerVectorV, &layerPowerVectorH,
                                     &(*gridVector));
                int afterSize = (*steinerLine).size();
                if (oriSize == afterSize) {
                    isValidRoute = false;
                }
            }
        }
    }
    //-------  routing by steiner point  end -------
    if (isValidRoute == false) {
        (*steinerLine).clear();
    }
}

int countDistance(SteinerPoint steinerPoint) {
    int totaly = steinerPoint.getCellPointCol() - steinerPoint.getSteinerPointCol();
    int totalx = steinerPoint.getCellPointRow() - steinerPoint.getSteinerPointRow();
    totaly = abs(totaly) + 1;
    totalx = abs(totalx);
    return (totaly + totalx);
};


int routOfDistance(vector<Route> routeVec) {
    int totalWireLength = 0;
    for (Route route : routeVec) {
        if (route.getStartLayIndx() == route.getEndlayIndx()) {
            int rowLength = route.getStartRowIndx() - route.getEndRowIndx();
            int colLength = route.getStartColIndx() - route.getEndColIndx();

            totalWireLength += abs(rowLength);
            totalWireLength += abs(colLength);
        }
    }
    return totalWireLength;
}


//判斷是否超過boundary
bool ReRoute::isOutOfBoundary(vector<Route> routeVec, map<string, int> boundaryMap) {
    bool isReRoute = false;
    for (Route route : routeVec) {
        if (route.getStartLayIndx() == route.getEndlayIndx()) {
            if (outOfBoundary(route, boundaryMap)) {
//                cout << route.getNetName() << endl;
//                cout << route.getStartRowIndx() << " " << route.getStartColIndx() << " " << route.getEndRowIndx() << " "
//                     << route.getEndColIndx() << endl;
                isReRoute = true;
                break;
            }
        }
    }
    return isReRoute;
}

int ReRoute::caculatehalfPerimeter(map<string, int> boundaryMap) {
    int halfPerimeter = 0;
    int verticalLength = boundaryMap[UP] - boundaryMap[DOWN];
    int horizontalLength = boundaryMap[LEFT] - boundaryMap[RIGHT];
    halfPerimeter = abs(verticalLength) + abs(horizontalLength);
    return halfPerimeter;
}

bool ReRoute::isOverFlowHalfPerimeter(vector<Route> routeVec, map<string, int> boundaryMap) {
    bool isReRoute = false;
    int totalWireLength = routOfDistance(routeVec);
    int halfPerimeter = caculatehalfPerimeter(boundaryMap);
    if (totalWireLength > halfPerimeter) {
        isReRoute = true;
    }
    return isReRoute;
}


//判斷每個點是否超過boudary
bool ReRoute::outOfBoundary(Route route, map<string, int> boundaryMap) {
    bool isReRoute = false;
    if (route.getStartRowIndx() > boundaryMap[UP] || route.getEndRowIndx() > boundaryMap[UP]) {
        isReRoute = true;
    }
    if (route.getStartRowIndx() < boundaryMap[DOWN] || route.getEndRowIndx() < boundaryMap[DOWN]) {
        isReRoute = true;
    }
    if (route.getStartColIndx() < boundaryMap[LEFT] || route.getEndColIndx() < boundaryMap[LEFT]) {
        isReRoute = true;
    }
    if (route.getStartColIndx() > boundaryMap[RIGHT] || route.getStartColIndx() > boundaryMap[RIGHT]) {
        isReRoute = true;
    }
    return isReRoute;
}

//取得Steiner Tree Routing 的線
void ReRoute::getSteinerRoute(vector<Route> *routeVector, string reRouteNet, map<string, Net> *netMap,
                              vector<vector<vector<int> > > *gridVector,
                              map<string, vector<int> > *powerFactorMap) {
    //TODO row 跟 col 這邊可能要注意一下
    int row[100], col[100];
    int index = 0;
    set<string> cellSet;
    //-------  steiner tree  start -------
    for (const auto &cell : (*netMap)[reRouteNet].getConnectCell()) {
        string cellString = to_string(cell.second.getRowIndx()) + "_" + to_string(cell.second.getColIndx());
        cellSet.insert(cellString);
        row[index] = cell.second.getRowIndx();
        col[index] = cell.second.getColIndx();
        index += 1;
    }
    //若有net中的cell都在同一點上，則無法重繞
    if (cellSet.size() <= 1) {
        return;
    }
    Flute::Tree flutetree;
    vector<RoutePoint> routePointVector;
    Flute::FluteState *flute1 = Flute::flute_init(FLUTE_POWVFILE, FLUTE_PORTFILE);
    flutetree = Flute::flute(flute1, index, row, col, FLUTE_ACCURACY);
    //-------  steiner tree  end -------
    transferFluteTree(&routePointVector, flutetree);

    //-------  steiner point route start -------
    //line for via
    map<string, vector<SteinerPoint> > layerSteinerMap;
    //point for via
    map<string, map<string, string> > pointMap;
    //steiner line for route
    vector<SteinerPoint> steinerLine;

    string minimumRoutingConstraint = (*netMap)[reRouteNet].getMinRoutingConstraint();
    getSteinerPointRoute(&routePointVector, &steinerLine, &(*gridVector), &(*powerFactorMap),
                         minimumRoutingConstraint, &layerSteinerMap, reRouteNet);
    free_tree(flute1, flutetree);
    flute_free(flute1);

    //-------  steiner point route end -------

    //-------  steiner point via start -------

    //----- put steiner Line in Map for via start -----
    // 將 steiner Line 放入 LayerSteinerMap 裡面去
    if (steinerLine.size() > 0) {
        //steiner Line
        for (const auto &steinerPoint : steinerLine) {
            //線放入route vector
            Route route;
            route.setStartLayIndx(steinerPoint.getLayer());
            route.setEndlayIndx(steinerPoint.getLayer());
            route.setStartRowIndx(steinerPoint.getCellPointRow());
            route.setEndRowIndx(steinerPoint.getSteinerPointRow());
            route.setStartColIndx(steinerPoint.getCellPointCol());
            route.setEndColIndx(steinerPoint.getSteinerPointCol());
            route.setNetName(reRouteNet);
            (*routeVector).push_back(route);
            //線 => steiner line
            if (layerSteinerMap.find(to_string(steinerPoint.getLayer())) == layerSteinerMap.end()) {
                vector<SteinerPoint> lineVector;
                lineVector.push_back(steinerPoint);
                layerSteinerMap.insert(
                        pair<string, vector<SteinerPoint> >(to_string(steinerPoint.getLayer()), lineVector));
            } else {
                layerSteinerMap[to_string(steinerPoint.getLayer())].push_back(steinerPoint);
            }
            //點 => 所有model上可以連的點
            string startCoordinate =
                    to_string(steinerPoint.getCellPointRow()) + "_" + to_string(steinerPoint.getCellPointCol());
            string endCoordinate =
                    to_string(steinerPoint.getSteinerPointRow()) + "_" +
                    to_string(steinerPoint.getSteinerPointCol());
            if (pointMap.find(to_string(steinerPoint.getLayer())) == pointMap.end()) {
                map<string, string> coordinateMap;
                coordinateMap.insert(pair<string, string>(startCoordinate, startCoordinate));
                coordinateMap.insert(pair<string, string>(endCoordinate, endCoordinate));
                pointMap.insert(
                        pair<string, map<string, string> >(to_string(steinerPoint.getLayer()), coordinateMap));
            } else {
                pointMap[to_string(steinerPoint.getLayer())].insert(
                        pair<string, string>(startCoordinate, startCoordinate));
                pointMap[to_string(steinerPoint.getLayer())].insert(
                        pair<string, string>(endCoordinate, endCoordinate));
            }
        };
        //----- steiner Line in Map start -----

        // ----- put cell in layerSteinerMap for via start -----
        // 所有的 cell 放到 layerSteinerMap
        for (const auto &cell:  (*netMap)[reRouteNet].getConnectCell()) {
            int layer = cell.second.getLayerName();
            int rowIndx = cell.second.getRowIndx();
            int colIndx = cell.second.getColIndx();
            bool isInside = false;
            for (SteinerPoint steiner : layerSteinerMap[to_string(layer)]) {
                int isHorizontal = layer % 2;
                //vertical
                if (isHorizontal == 0) {
                    if (steiner.getCellPointCol() > steiner.getSteinerPointCol()) {
                        if (colIndx <= steiner.getCellPointCol() or colIndx >= steiner.getSteinerPointCol()) {
                            bool isInside = true;
                            break;
                        }
                    } else {
                        if (colIndx >= steiner.getCellPointCol() or colIndx <= steiner.getSteinerPointCol()) {
                            bool isInside = true;
                            break;
                        }
                    }
                    //horizontal
                } else {
                    if (steiner.getCellPointRow() > steiner.getSteinerPointRow()) {
                        if (rowIndx <= steiner.getCellPointRow() or rowIndx >= steiner.getSteinerPointRow()) {
                            bool isInside = true;
                            break;
                        }
                    } else {
                        if (rowIndx >= steiner.getCellPointRow() or rowIndx <= steiner.getSteinerPointRow()) {
                            bool isInside = true;
                            break;
                        }
                    }

                }
            };
            if (isInside == false) {
                SteinerPoint steinerPoint(rowIndx, colIndx, rowIndx, colIndx, layer);
                layerSteinerMap[to_string(steinerPoint.getLayer())].push_back(steinerPoint);
            }
        }
        // ----- put cell in layerSteinerMap for via end -----
//        layerSteinerMap -> 所有steiner Line 包括 cell  :
//        pointMap ->        所有steiner Line 的點       :  key -> layer
        set<string> viaSet;
        bool isValidVia = true;
        //每一層
        for (const auto &layerMap : layerSteinerMap) {
            string layer = layerMap.first;
            bool getVia = false;
            //每一層的線或cell
            for (int i = 0; i < layerMap.second.size(); i++) {
                SteinerPoint steiner = layerMap.second[i];
                string steinerCoordinate =
                        to_string(steiner.getSteinerPointRow()) + "_" + to_string(steiner.getSteinerPointCol());
                string cellCoordinate =
                        to_string(steiner.getCellPointRow()) + "_" + to_string(steiner.getCellPointCol());
                //point map 從第二層開始
                for (const auto &pointGridMap : pointMap) {
                    int pointLayer = stoi(pointGridMap.first);
                    if (pointLayer == steiner.getLayer()) {
                        continue;
                    }
                    //point 跟 layer同一層不用判斷
                    if (pointMap[to_string(pointLayer)].count(steinerCoordinate) > 0) {
                        if (isRepeatVia(steinerCoordinate, layer, to_string(pointLayer), &viaSet) == false) {
                            //確認中間via 是否有supply 不足的情況
                            if (isViaSupplyValidFunction(stoi(layer), pointLayer, steiner.getSteinerPointRow(),
                                                         steiner.getSteinerPointCol(), *gridVector) == true) {
                                Route route;
                                route.setStartLayIndx(stoi(layer));
                                route.setEndlayIndx(pointLayer);
                                route.setStartRowIndx(steiner.getSteinerPointRow());
                                route.setEndRowIndx(steiner.getSteinerPointRow());
                                route.setStartColIndx(steiner.getSteinerPointCol());
                                route.setEndColIndx(steiner.getSteinerPointCol());
                                route.setNetName(reRouteNet);
                                (*routeVector).push_back(route);
                                //                                cout << "route line : " << route.getStartRowIndx() << " "
//                                     << route.getStartColIndx() << " " << route.getStartLayIndx()
//                                     << " " << route.getEndRowIndx() << " " << route.getEndColIndx() << " "
//                                     << route.getEndlayIndx() << " "
//                                     << route.getNetName()
//                                     << endl;
                            } else {
                                isValidVia = false;
                                break;
                            }
                        }
                    } else if (pointMap[to_string(pointLayer)].count(cellCoordinate) > 0) {
                        if (isRepeatVia(cellCoordinate, layer, to_string(pointLayer), &viaSet) == false) {
                            //確認中間via 是否有supply 不足的情況
                            if (isViaSupplyValidFunction(stoi(layer), pointLayer, steiner.getCellPointRow(),
                                                         steiner.getCellPointCol(), *gridVector)) {
                                Route route;
                                route.setStartLayIndx(stoi(layer));
                                route.setEndlayIndx(pointLayer);
                                route.setStartRowIndx(steiner.getCellPointRow());
                                route.setEndRowIndx(steiner.getCellPointRow());
                                route.setStartColIndx(steiner.getCellPointCol());
                                route.setEndColIndx(steiner.getCellPointCol());
                                route.setNetName(reRouteNet);
                                (*routeVector).push_back(route);
                            } else {
                                isValidVia = false;
                                break;
                            }
                        }
                    }
                }
                if (isValidVia == false) {
                    break;
                }
            }

            if (isValidVia == false) {
                break;
            }
        }

        // via end
        if (isValidVia == false) {

            (*routeVector).clear();
        }
        //-------  steiner point via start -------
//            cout << "End route line : " << endl;
//            for (Route route: (*routeVector)) {
//                cout << "route line : " << route.getStartRowIndx() << " "
//                     << route.getStartColIndx() << " " << route.getStartLayIndx()
//                     << " " << route.getEndRowIndx() << " " << route.getEndColIndx() << " " << route.getEndlayIndx()
//                     << " "
//                     << route.getNetName()
//                     << endl;
//            }
    }

}

bool ReRoute::isViaSupplyValidFunction(int startLayer, int endLayer, int row, int col,
                                       vector<vector<vector<int> > > gridVector) {
    bool isViaSupplyValid = true;
    if (startLayer < endLayer) {
        for (int layerIndex = startLayer; layerIndex <= endLayer; layerIndex++) {
            if (gridVector[layerIndex - 1][row - 1][col - 1] <= 0) {
                isViaSupplyValid = false;
                break;
            }
        }
    } else {
        for (int layerIndex = endLayer; layerIndex <= startLayer; layerIndex++) {
            if (gridVector[layerIndex - 1][row - 1][col - 1] <= 0) {
                isViaSupplyValid = false;
                break;
            }

        }

    }
    return isViaSupplyValid;
}


//判斷是否有重複的via
//    string via = steinerCoordinate + "_" + layer + "_" + to_string(pointLayer);
bool ReRoute::isRepeatVia(string coordinate, string upLayer, string downLayer, set<string> *viaSet) {
    string viaUp = coordinate + "_" + upLayer + "_" + downLayer;
    string viaDown = coordinate + "_" + downLayer + "_" + upLayer;
    if ((*viaSet).count(viaUp) > 0 or (*viaSet).count(viaDown) > 0) {
        return true;
    } else {
        (*viaSet).insert(viaUp);
        (*viaSet).insert(viaDown);
        return false;
    }
}

bool ReRoute::isRevise(string point, set<string> *routeSet) {
    if ((*routeSet).count(point) > 0) {
        return true;
    } else {
        (*routeSet).insert(point);
        return false;
    }
}

void ReRoute::topRightToBottomLeft(vector<SteinerPoint> *steinerLineVector, int startRowGrid, int endRowGrid,
                                   int startColGrid,
                                   int endColGrid, vector<int> *layerPowerVectorV, vector<int> *layerPowerVectorH,
                                   vector<vector<vector<int> > > *gridVector) {
    SteinerPoint steinerPointFirst;
    SteinerPoint steinerPointSecond;
    SteinerPoint steinerPointThird;
    bool foundRoute = false;
    //down-left-down
    for (int tempCol = startColGrid; tempCol >= endColGrid; tempCol--) {
        bool lineFirst = false;
        bool lineSecond = false;
        bool lineThird = false;

        if (lineFirst == false) {
            for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                bool lackSupply = false;
                for (int upCol = startColGrid; upCol >= tempCol; upCol--) {
//                        cout << "up :" << gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][upCol] << endl;
                    if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][startRowGrid][upCol] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointFirst.setLayer((*layerPowerVectorH)[layer]);
                    steinerPointFirst.setCellPointRow(startRowGrid + 1);
                    steinerPointFirst.setCellPointCol(startColGrid + 1);
                    steinerPointFirst.setSteinerPointRow(startRowGrid + 1);
                    steinerPointFirst.setSteinerPointCol(tempCol + 1);
                    lineFirst = true;
//                        cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
//                             << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
//                             << " "
//                             << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer() << endl;
                    break;
                }
            }
        }

        if (lineSecond == false) {
            for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                bool lackSupply = false;
                for (int row = startRowGrid; row >= endRowGrid; row--) {
//                        cout << "V :" << gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] << endl;
                    if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][row][tempCol] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointSecond.setLayer((*layerPowerVectorV)[layer]);
                    steinerPointSecond.setCellPointRow(startRowGrid + 1);
                    steinerPointSecond.setCellPointCol(tempCol + 1);
                    steinerPointSecond.setSteinerPointRow(endRowGrid + 1);
                    steinerPointSecond.setSteinerPointCol(tempCol + 1);
//                        cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
//                             << steinerPointSecond.getCellPointCol() << " " << steinerPointSecond.getSteinerPointRow()
//                             << " "
//                             << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer() << endl;
                    lineSecond = true;
                    break;
                }
            }
        }
        if (lineThird == false) {
            for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                bool lackSupply = false;
                for (int downCol = tempCol; downCol >= endColGrid; downCol--) {
//                        cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][downCol]
//                             << endl;
                    if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][endRowGrid][downCol] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointThird.setLayer((*layerPowerVectorH)[layer]);
                    steinerPointThird.setCellPointRow(endRowGrid + 1);
                    steinerPointThird.setCellPointCol(tempCol + 1);
                    steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                    steinerPointThird.setSteinerPointCol(endColGrid + 1);
//                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
//                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
//                             << " "
//                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
                    lineThird = true;
                    break;
                }

            }
        }
        if (lineFirst == true and lineSecond == true and lineThird == true) {
            if (countDistance(steinerPointFirst) > 1) {
                (*steinerLineVector).push_back(steinerPointFirst);
            }
            if (countDistance(steinerPointSecond) > 1) {
                (*steinerLineVector).push_back(steinerPointSecond);
            }
            if (countDistance(steinerPointThird) > 1) {
                (*steinerLineVector).push_back(steinerPointThird);
            }
            foundRoute = true;
//                cout << "Steiner line vector :" << endl;
//                for (int i = 0; i < steinerLineVector.size(); i++) {
//                    cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
//                         << " " << steinerLineVector[i].getSteinerPointRow() << " "
//                         << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer() << endl;
//                }
            break;
        }
    }
    if (foundRoute == false) {
        //Right-H-Right
        for (int tempRow = startRowGrid; tempRow >= endRowGrid; tempRow--) {
            bool lineFirst = false;
            bool lineSecond = false;
            bool lineThird = false;

            if (lineFirst == false) {
                for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                    bool lackSupply = false;
                    for (int rightRow = startRowGrid; rightRow >= tempRow; rightRow--) {
//                            cout << "left :" << gridVector[(layerPowerVectorV[layer] - 1)][rightRow][startColGrid]
//                                 << endl;
                        if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][rightRow][startColGrid] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointFirst.setLayer((*layerPowerVectorV)[layer]);
                        steinerPointFirst.setCellPointRow(startRowGrid + 1);
                        steinerPointFirst.setCellPointCol(startColGrid + 1);
                        steinerPointFirst.setSteinerPointRow(tempRow + 1);
                        steinerPointFirst.setSteinerPointCol(startColGrid + 1);
                        lineFirst = true;
//                            cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
//                                 << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer()
//                                 << endl;
                        break;
                    }
                }
            }

            if (lineSecond == false) {
                for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                    bool lackSupply = false;
                    for (int col = startColGrid; col >= endColGrid; col--) {
//                            cout << "H :" << gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] << endl;
                        if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][tempRow][col] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointSecond.setLayer((*layerPowerVectorH)[layer]);
                        steinerPointSecond.setCellPointRow(tempRow + 1);
                        steinerPointSecond.setCellPointCol(startColGrid + 1);
                        steinerPointSecond.setSteinerPointRow(tempRow + 1);
                        steinerPointSecond.setSteinerPointCol(endColGrid + 1);
//                            cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
//                                 << steinerPointSecond.getCellPointCol() << " "
//                                 << steinerPointSecond.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer()
//                                 << endl;
                        lineSecond = true;
                        break;
                    }
                }
            }

            if (lineThird == false) {
                for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                    bool lackSupply = false;
                    for (int leftRow = tempRow; leftRow >= endRowGrid; leftRow--) {
//                            cout << "right :" << gridVector[(layerPowerVectorV[layer] - 1)][leftRow][endColGrid]
//                                 << endl;
                        if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][leftRow][endColGrid] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointThird.setLayer((*layerPowerVectorV)[layer]);
                        steinerPointThird.setCellPointRow(tempRow + 1);
                        steinerPointThird.setCellPointCol(endColGrid + 1);
                        steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                        steinerPointThird.setSteinerPointCol(endColGrid + 1);
//                            cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
//                                 << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer()
//                                 << endl;
                        lineThird = true;
                        break;
                    }
                }
            }
//                cout << "" << endl;
            if (lineFirst == true and lineSecond == true and lineThird == true) {
                if (countDistance(steinerPointFirst) > 1) {
                    (*steinerLineVector).push_back(steinerPointFirst);
                }
                if (countDistance(steinerPointSecond) > 1) {
                    (*steinerLineVector).push_back(steinerPointSecond);
                }
                if (countDistance(steinerPointThird) > 1) {
                    (*steinerLineVector).push_back(steinerPointThird);
                }
                foundRoute = true;
//                    cout << "Steiner line vector :" << endl;
//                    for (int i = 0; i < steinerLineVector.size(); i++) {
//                        cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
//                             << " " << steinerLineVector[i].getSteinerPointRow() << " "
//                             << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
//                             << endl;
//                    }
                break;
            }
        }
    }
}


void ReRoute::topLeftToBottomRight(vector<SteinerPoint> *steinerLineVector, int startRowGrid, int endRowGrid,
                                   int startColGrid,
                                   int endColGrid, vector<int> *layerPowerVectorV, vector<int> *layerPowerVectorH,
                                   vector<vector<vector<int> > > *gridVector) {
    SteinerPoint steinerPointFirst;
    SteinerPoint steinerPointSecond;
    SteinerPoint steinerPointThird;
    bool foundRoute = false;

    //Right-down-Right
    for (int tempCol = startColGrid; tempCol <= endColGrid; tempCol++) {
        bool lineFirst = false;
        bool lineSecond = false;
        bool lineThird = false;

        if (lineFirst == false) {
            for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                bool lackSupply = false;
                for (int upCol = startColGrid; upCol <= tempCol; upCol++) {
//                        cout << "up :" << gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][upCol] << endl;
                    if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][startRowGrid][upCol] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointFirst.setLayer((*layerPowerVectorH)[layer]);
                    steinerPointFirst.setCellPointRow(startRowGrid + 1);
                    steinerPointFirst.setCellPointCol(startColGrid + 1);
                    steinerPointFirst.setSteinerPointRow(startRowGrid + 1);
                    steinerPointFirst.setSteinerPointCol(tempCol + 1);
                    lineFirst = true;
//                        cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
//                             << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
//                             << " "
//                             << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer() << endl;
                    break;
                }
            }
        }

        if (lineSecond == false) {
            for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                bool lackSupply = false;
                for (int row = startRowGrid; row >= endRowGrid; row--) {
//                        cout << "V :" << gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] << endl;
                    if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][row][tempCol] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointSecond.setLayer((*layerPowerVectorV)[layer]);
                    steinerPointSecond.setCellPointRow(startRowGrid + 1);
                    steinerPointSecond.setCellPointCol(tempCol + 1);
                    steinerPointSecond.setSteinerPointRow(endRowGrid + 1);
                    steinerPointSecond.setSteinerPointCol(tempCol + 1);
//                        cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
//                             << steinerPointSecond.getCellPointCol() << " " << steinerPointSecond.getSteinerPointRow()
//                             << " "
//                             << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer() << endl;
                    lineSecond = true;
                    break;
                }
            }
        }

        if (lineThird == false) {
            for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                bool lackSupply = false;
                for (int downCol = tempCol; downCol <= endColGrid; downCol++) {
//                        cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][downCol]
//                             << endl;
                    if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][endRowGrid][downCol] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointThird.setLayer((*layerPowerVectorH)[layer]);
                    steinerPointThird.setCellPointRow(endRowGrid + 1);
                    steinerPointThird.setCellPointCol(tempCol + 1);
                    steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                    steinerPointThird.setSteinerPointCol(endColGrid + 1);
//                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
//                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
//                             << " "
//                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
                    lineThird = true;
                    break;
                }
            }
        }
//            cout << "" << endl;
        if (lineFirst == true and lineSecond == true and lineThird == true) {
            if (countDistance(steinerPointFirst) > 1) {
                (*steinerLineVector).push_back(steinerPointFirst);
            }
            if (countDistance(steinerPointSecond) > 1) {
                (*steinerLineVector).push_back(steinerPointSecond);
            }
            if (countDistance(steinerPointThird) > 1) {
                (*steinerLineVector).push_back(steinerPointThird);
            }
            foundRoute = true;
//                cout << "Steiner line vector :" << endl;
//                for (int i = 0; i < steinerLineVector.size(); i++) {
//                    cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
//                         << " " << steinerLineVector[i].getSteinerPointRow() << " "
//                         << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
//                         << endl;
//                }
            break;
        }
    }


    if (foundRoute == false) {
        //down-Right-down
        for (int tempRow = startRowGrid; tempRow >= endRowGrid; tempRow--) {
            bool lineFirst = false;
            bool lineSecond = false;
            bool lineThird = false;
            if (lineFirst == false) {
                for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                    bool lackSupply = false;
                    for (int leftRow = startRowGrid; leftRow >= tempRow; leftRow--) {
//                            cout << "left :" << gridVector[(layerPowerVectorV[layer] - 1)][leftRow][startColGrid]
//                                 << endl;
                        if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][leftRow][startColGrid] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointFirst.setLayer((*layerPowerVectorV)[layer]);
                        steinerPointFirst.setCellPointRow(startRowGrid + 1);
                        steinerPointFirst.setCellPointCol(startColGrid + 1);
                        steinerPointFirst.setSteinerPointRow(tempRow + 1);
                        steinerPointFirst.setSteinerPointCol(startColGrid + 1);
                        lineFirst = true;
//                            cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
//                                 << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer()
//                                 << endl;
                        break;
                    }
                }
            }


            if (lineSecond == false) {
                for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                    bool lackSupply = false;
                    for (int col = startColGrid; col <= endColGrid; col++) {
//                            cout << "H :" << gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] << endl;
                        if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][tempRow][col] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointSecond.setLayer((*layerPowerVectorH)[layer]);
                        steinerPointSecond.setCellPointRow(tempRow + 1);
                        steinerPointSecond.setCellPointCol(startColGrid + 1);
                        steinerPointSecond.setSteinerPointRow(tempRow + 1);
                        steinerPointSecond.setSteinerPointCol(endColGrid + 1);
//                            cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
//                                 << steinerPointSecond.getCellPointCol() << " "
//                                 << steinerPointSecond.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer()
//                                 << endl;
                        lineSecond = true;
                        break;
                    }
                }
            }

            if (lineThird == false) {
                for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                    bool lackSupply = false;
                    for (int rightRow = tempRow; rightRow >= endRowGrid; rightRow--) {
//                            cout << "right :" << gridVector[(layerPowerVectorV[layer] - 1)][rightRow][endColGrid]
//                                 << endl;
                        if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][rightRow][endColGrid] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointThird.setLayer((*layerPowerVectorV)[layer]);
                        steinerPointThird.setCellPointRow(tempRow + 1);
                        steinerPointThird.setCellPointCol(endColGrid + 1);
                        steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                        steinerPointThird.setSteinerPointCol(endColGrid + 1);
//                            cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
//                                 << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer()
//                                 << endl;
                        lineThird = true;
                        break;
                    }
                }
                if (lineFirst == true and lineSecond == true and lineThird == true) {
                    if (countDistance(steinerPointFirst) > 1) {
                        (*steinerLineVector).push_back(steinerPointFirst);
                    }
                    if (countDistance(steinerPointSecond) > 1) {
                        (*steinerLineVector).push_back(steinerPointSecond);
                    }
                    if (countDistance(steinerPointThird) > 1) {
                        (*steinerLineVector).push_back(steinerPointThird);
                    }
                    foundRoute = true;
//                        cout << "Steiner line vector :" << endl;
//                        for (int i = 0; i < steinerLineVector.size(); i++) {
//                            cout << steinerLineVector[i].getCellPointRow() << " "
//                                 << steinerLineVector[i].getCellPointCol()
//                                 << " " << steinerLineVector[i].getSteinerPointRow() << " "
//                                 << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
//                                 << endl;
//                        }
                    break;
                }
            }
//                cout << "" << endl;
        }
    }
}


void ReRoute::bottomRightToTopLeft(vector<SteinerPoint> *steinerLineVector, int startRowGrid, int endRowGrid,
                                   int startColGrid,
                                   int endColGrid, vector<int> *layerPowerVectorV, vector<int> *layerPowerVectorH,
                                   vector<vector<vector<int> > > *gridVector) {
    SteinerPoint steinerPointFirst;
    SteinerPoint steinerPointSecond;
    SteinerPoint steinerPointThird;
    bool foundRoute = false;
    //右下到左上
//        cout << " Bottom right to top left" << endl;
    //Up-Right-Up
    for (int tempRow = startRowGrid; tempRow <= endRowGrid; tempRow++) {
        bool lineFirst = false;
        bool lineSecond = false;
        bool lineThird = false;
        if (lineFirst == false) {
            for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                bool lackSupply = false;
                for (int rightRow = startRowGrid; rightRow <= tempRow; rightRow++) {
//                        cout << "right :" << gridVector[(layerPowerVectorV[layer] - 1)][rightRow][startColGrid]
//                             << endl;
                    if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][rightRow][startColGrid] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointFirst.setLayer((*layerPowerVectorV)[layer]);
                    steinerPointFirst.setCellPointRow(startRowGrid + 1);
                    steinerPointFirst.setCellPointCol(startColGrid + 1);
                    steinerPointFirst.setSteinerPointRow(tempRow + 1);
                    steinerPointFirst.setSteinerPointCol(startColGrid + 1);
                    lineFirst = true;
//                        cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
//                             << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
//                             << " "
//                             << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer() << endl;
                    break;
                }
            }
        }

        if (lineSecond == false) {
            for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                bool lackSupply = false;
                for (int col = startColGrid; col >= endColGrid; col--) {
//                        cout << "H :" << gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] << endl;
                    if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][tempRow][col] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointSecond.setLayer((*layerPowerVectorH)[layer]);
                    steinerPointSecond.setCellPointRow(tempRow + 1);
                    steinerPointSecond.setCellPointCol(startColGrid + 1);
                    steinerPointSecond.setSteinerPointRow(tempRow + 1);
                    steinerPointSecond.setSteinerPointCol(endColGrid + 1);
//                        cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
//                             << steinerPointSecond.getCellPointCol() << " " << steinerPointSecond.getSteinerPointRow()
//                             << " "
//                             << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer() << endl;
                    lineSecond = true;
                    break;
                }
            }
        }

        if (lineThird == false) {
            for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                bool lackSupply = false;
                for (int leftRow = tempRow; leftRow <= endRowGrid; leftRow++) {
//                        cout << "left :" << gridVector[(layerPowerVectorV[layer] - 1)][leftRow][endColGrid]
//                             << endl;
                    if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][leftRow][endColGrid] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointThird.setLayer((*layerPowerVectorV)[layer]);
                    steinerPointThird.setCellPointRow(tempRow + 1);
                    steinerPointThird.setCellPointCol(endColGrid + 1);
                    steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                    steinerPointThird.setSteinerPointCol(endColGrid + 1);
//                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
//                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
//                             << " "
//                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
                    lineThird = true;
                    break;
                }
            }
        }

//            cout << "" << endl;
        if (lineFirst == true and lineSecond == true and lineThird == true) {
            if (countDistance(steinerPointFirst) > 1) {
                (*steinerLineVector).push_back(steinerPointFirst);
            }
            if (countDistance(steinerPointSecond) > 1) {
                (*steinerLineVector).push_back(steinerPointSecond);
            }
            if (countDistance(steinerPointThird) > 1) {
                (*steinerLineVector).push_back(steinerPointThird);
            }
            foundRoute = true;
//                cout << "Steiner line vector :" << endl;
//                for (int i = 0; i < steinerLineVector.size(); i++) {
//                    cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
//                         << " " << steinerLineVector[i].getSteinerPointRow() << " "
//                         << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer() << endl;
//                }
            break;
        }
    }

    if (foundRoute == false) {
        //Right-Up-Right
        for (int tempCol = startColGrid; tempCol >= endColGrid; tempCol--) {
            bool lineFirst = false;
            bool lineSecond = false;
            bool lineThird = false;

            if (lineFirst == false) {
                for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                    bool lackSupply = false;
                    for (int downCol = startColGrid; downCol >= tempCol; downCol--) {
//                            cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][downCol]
//                                 << endl;
                        if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][startRowGrid][downCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointFirst.setLayer((*layerPowerVectorH)[layer]);
                        steinerPointFirst.setCellPointRow(startRowGrid + 1);
                        steinerPointFirst.setCellPointCol(startColGrid + 1);
                        steinerPointFirst.setSteinerPointRow(startRowGrid + 1);
                        steinerPointFirst.setSteinerPointCol(tempCol + 1);
                        lineFirst = true;
//                            cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
//                                 << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer()
//                                 << endl;
                        break;
                    }
                }
            }

            if (lineSecond == false) {
                bool lackSupply = false;
                for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                    for (int row = startRowGrid; row <= endRowGrid; row++) {
//                            cout << "V :" << gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] << endl;
                        if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][row][tempCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointSecond.setLayer((*layerPowerVectorV)[layer]);
                        steinerPointSecond.setCellPointRow(startRowGrid + 1);
                        steinerPointSecond.setCellPointCol(tempCol + 1);
                        steinerPointSecond.setSteinerPointRow(endRowGrid + 1);
                        steinerPointSecond.setSteinerPointCol(tempCol + 1);
//                            cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
//                                 << steinerPointSecond.getCellPointCol() << " "
//                                 << steinerPointSecond.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer()
//                                 << endl;
                        lineSecond = true;
                        break;
                    }
                }
            }


            if (lineThird == false) {
                bool lackSupply = false;
                for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                    for (int upCol = tempCol; upCol >= endColGrid; upCol--) {
//                            cout << "up :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][upCol] << endl;
                        if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][endRowGrid][upCol] <= 0) {
                            lackSupply = true;
                        }
                    }

                    if (lackSupply == false) {
                        steinerPointThird.setLayer((*layerPowerVectorH)[layer]);
                        steinerPointThird.setCellPointRow(endRowGrid + 1);
                        steinerPointThird.setCellPointCol(tempCol + 1);
                        steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                        steinerPointThird.setSteinerPointCol(endColGrid + 1);
//                            cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
//                                 << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer()
//                                 << endl;
                        lineThird = true;
                        break;
                    }
                }
            }
//                cout << "" << endl;
            if (lineFirst == true and lineSecond == true and lineThird == true) {
                if (countDistance(steinerPointFirst) > 1) {
                    (*steinerLineVector).push_back(steinerPointFirst);
                }
                if (countDistance(steinerPointSecond) > 1) {
                    (*steinerLineVector).push_back(steinerPointSecond);
                }
                if (countDistance(steinerPointThird) > 1) {
                    (*steinerLineVector).push_back(steinerPointThird);
                }
                foundRoute = true;
//                    cout << "Steiner line vector :" << endl;
//                    for (int i = 0; i < steinerLineVector.size(); i++) {
//                        cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
//                             << " " << steinerLineVector[i].getSteinerPointRow() << " "
//                             << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
//                             << endl;
//                    }
                break;
            }
        }
    }
}


void ReRoute::bottomLeftToTopRight(vector<SteinerPoint> *steinerLineVector, int startRowGrid, int endRowGrid,
                                   int startColGrid,
                                   int endColGrid, vector<int> *layerPowerVectorV, vector<int> *layerPowerVectorH,
                                   vector<vector<vector<int> > > *gridVector) {
    SteinerPoint steinerPointFirst;
    SteinerPoint steinerPointSecond;
    SteinerPoint steinerPointThird;
    bool foundRoute = false;
    //Up-Right-Up
    //向上走 每換一次Row 代表 換不同的 pattern route 的 方法
    for (int tempRow = startRowGrid; tempRow <= endRowGrid; tempRow++) {
        bool lineFirst = false;
        bool lineSecond = false;
        bool lineThird = false;
        //當此pattern 可以找到不同層的線，即可完成pattern Route
        if (lineFirst == false) {
            for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                bool lackSupply = false;
                for (int leftRow = startRowGrid; leftRow <= tempRow; leftRow++) {
//                        cout << "left :" << gridVector[(layerPowerVectorV[layer]) - 1][leftRow][startColGrid] << endl;
                    if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][leftRow][startColGrid] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointFirst.setLayer((*layerPowerVectorV)[layer]);
                    steinerPointFirst.setCellPointRow(startRowGrid + 1);
                    steinerPointFirst.setCellPointCol(startColGrid + 1);
                    steinerPointFirst.setSteinerPointRow(tempRow + 1);
                    steinerPointFirst.setSteinerPointCol(startColGrid + 1);
                    lineFirst = true;
//                        cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
//                             << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
//                             << " "
//                             << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer() << endl;
                    break;
                }
            }
        }
        //向右走
        if (lineSecond == false) {
            for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                bool lackSupply = false;
                for (int col = startColGrid; col <= endColGrid; col++) {
//                        cout << "H :" << gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] << endl;
                    if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][tempRow][col] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointSecond.setLayer((*layerPowerVectorH)[layer]);
                    steinerPointSecond.setCellPointRow(tempRow + 1);
                    steinerPointSecond.setCellPointCol(startColGrid + 1);
                    steinerPointSecond.setSteinerPointRow(tempRow + 1);
                    steinerPointSecond.setSteinerPointCol(endColGrid + 1);
//                        cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
//                             << steinerPointSecond.getCellPointCol() << " " << steinerPointSecond.getSteinerPointRow()
//                             << " "
//                             << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer() << endl;
                    lineSecond = true;
                    break;
                }
            }
        }
        //向上走
        if (lineThird == false) {
            for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                bool lackSupply = false;
                for (int rightRow = tempRow; rightRow <= endRowGrid; rightRow++) {
//                        cout << "right :" << gridVector[(layerPowerVectorV[layer] - 1)][rightRow][endColGrid]
//                             << endl;
                    if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][rightRow][endColGrid] <= 0) {
                        lackSupply = true;
                    }
                }
                if (lackSupply == false) {
                    steinerPointThird.setLayer((*layerPowerVectorV)[layer]);
                    steinerPointThird.setCellPointRow(tempRow + 1);
                    steinerPointThird.setCellPointCol(endColGrid + 1);
                    steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                    steinerPointThird.setSteinerPointCol(endColGrid + 1);
//                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
//                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
//                             << " "
//                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
                    lineThird = true;
                    break;
                }
            }
        }
//            cout << "" << endl;
        if (lineFirst == true and lineSecond == true and lineThird == true) {
            if (countDistance(steinerPointFirst) > 1) {
                (*steinerLineVector).push_back(steinerPointFirst);
            }
            if (countDistance(steinerPointSecond) > 1) {
                (*steinerLineVector).push_back(steinerPointSecond);
            }
            if (countDistance(steinerPointThird) > 1) {
                (*steinerLineVector).push_back(steinerPointThird);
            }
            foundRoute = true;
//                cout << "Steiner line vector :" << endl;
//                for (int i = 0; i < steinerLineVector.size(); i++) {
//                    cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
//                         << " " << steinerLineVector[i].getSteinerPointRow() << " "
//                         << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer() << endl;
//                }
            break;
        }
    }

    if (foundRoute == false) {
        // Right-Up-Right
        for (int tempCol = startColGrid; tempCol <= endColGrid; tempCol++) {
            bool lineFirst = false;
            bool lineSecond = false;
            bool lineThird = false;
            if (lineFirst == false) {
                for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                    bool lackSupply = false;
                    for (int downCol = startColGrid; downCol <= tempCol; downCol++) {
//                            cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][downCol]
//                                 << endl;
                        if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][startRowGrid][downCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointFirst.setLayer((*layerPowerVectorH)[layer]);
                        steinerPointFirst.setCellPointRow(startRowGrid + 1);
                        steinerPointFirst.setCellPointCol(startColGrid + 1);
                        steinerPointFirst.setSteinerPointRow(startRowGrid + 1);
                        steinerPointFirst.setSteinerPointCol(tempCol + 1);
                        lineFirst = true;
//                            cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
//                                 << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer()
//                                 << endl;
                        break;
                    }
                }
            }
            if (lineSecond == false) {
                for (int layer = 0; layer < (*layerPowerVectorV).size(); layer++) {
                    bool lackSupply = false;
                    for (int row = startRowGrid; row <= endRowGrid; row++) {
//                            cout << "V :" << gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] << endl;
                        if ((*gridVector)[((*layerPowerVectorV)[layer] - 1)][row][tempCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointSecond.setLayer((*layerPowerVectorV)[layer]);
                        steinerPointSecond.setCellPointRow(startRowGrid + 1);
                        steinerPointSecond.setCellPointCol(tempCol + 1);
                        steinerPointSecond.setSteinerPointRow(endRowGrid + 1);
                        steinerPointSecond.setSteinerPointCol(tempCol + 1);
//                            cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
//                                 << steinerPointSecond.getCellPointCol() << " "
//                                 << steinerPointSecond.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer()
//                                 << endl;
                        lineSecond = true;
                        break;
                    }
                }
            }
            if (lineThird == false) {
                for (int layer = 0; layer < (*layerPowerVectorH).size(); layer++) {
                    bool lackSupply = false;
                    for (int upCol = tempCol; upCol <= endColGrid; upCol++) {
//                            cout << "up :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][upCol] << endl;
                        if ((*gridVector)[((*layerPowerVectorH)[layer] - 1)][endRowGrid][upCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointThird.setLayer((*layerPowerVectorH)[layer]);
                        steinerPointThird.setCellPointRow(endRowGrid + 1);
                        steinerPointThird.setCellPointCol(tempCol + 1);
                        steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                        steinerPointThird.setSteinerPointCol(endColGrid + 1);
//                            cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
//                                 << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
//                                 << " "
//                                 << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer()
//                                 << endl;
                        lineThird = true;
                        break;
                    }
                }
            }
//                cout << "" << endl;
            if (lineFirst == true and lineSecond == true and lineThird == true) {
                if (countDistance(steinerPointFirst) > 1) {
                    (*steinerLineVector).push_back(steinerPointFirst);
                }
                if (countDistance(steinerPointSecond) > 1) {
                    (*steinerLineVector).push_back(steinerPointSecond);
                }
                if (countDistance(steinerPointThird) > 1) {
                    (*steinerLineVector).push_back(steinerPointThird);
                }
                foundRoute = true;
//                    cout << "Steiner line vector :" << endl;
//                    for (int i = 0; i < steinerLineVector.size(); i++) {
//                        cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
//                             << " " << steinerLineVector[i].getSteinerPointRow() << " "
//                             << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
//                             << endl;
//                    }
                break;
            }

        }
    }
}



//    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
//        bool lackSupply = false;
//        for (int downCol = tempCol; downCol >= endColGrid; downCol--) {
////                        cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][downCol]
////                             << endl;
//            if (gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][downCol] <= 0) {
//                lackSupply = true;
//            }
//        }
//        if (lackSupply == false) {
//            steinerPointThird.setLayer(layerPowerVectorH[layer]);
//            steinerPointThird.setCellPointRow(endRowGrid + 1);
//            steinerPointThird.setCellPointCol(tempCol + 1);
//            steinerPointThird.setSteinerPointRow(endRowGrid + 1);
//            steinerPointThird.setSteinerPointCol(endColGrid + 1);
////                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
////                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
////                             << " "
////                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
//            lineThird = true;
//            break;
//        }
//
//    }

//void UPatternRouteV(int startRow, int endRow, int col, vector<SteinerPoint> *steinerLineVector,
//                    vector<vector<vector<int>>> gridVector, vector<int> layerPowerVectorV,
//                    vector<int> layerPowerVectorH) {
//    //不用減 1 已經減好了
//    bool canRoute = true;
//
//    //右 start U-pattern
//    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
//
//    }
//
//    //右 end U-pattern
//
//    //左 start U-pattern
//
//    //左 end U-pattern
//
//    //直 U-pattern
//    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
//        if (startRow < endRow) {
//            for (int row = startRow; row <= endRow; row++) {
//                if (gridVector[(layerPowerVectorV[layer] - 1)][row][col] <= 0) {
//                    canRoute = false;
//                }
//            }
//        } else {
//            for (int row = endRow; row <= startRow; row++) {
//                if (gridVector[(layerPowerVectorV[layer] - 1)][row][col] <= 0) {
//                    canRoute = false;
//                }
//            }
//
//        }
//    }
//
//
//}

//if (item.first == "N78" or item.first == "N792" or item.first == "N1661" or
//item.first == "N1662" or item.first == "N1665" or item.first == "N1698" or
//item.first == "N1701" or item.first == "N1743" or item.first == "N1838" or
//item.first == "N1843" or item.first == "N1845" or item.first == "N1861" or
//item.first== "N1885" or item.first == "N1934") {
//myfile << "gridSupply : " << (*gridVector)[1 - 1][2 - 1][5 - 1] << endl;
//}


//            // layerMap
//            cout << "layerMap :" << endl;
//            for (auto const item : layerSteinerMap) {
//                for (auto stei : item.second) {
//                    cout << stei.getCellPointRow() << " " << stei.getCellPointCol() << " " << stei.getSteinerPointRow()
//                         << " " << stei.getSteinerPointCol() << " " << stei.getLayer() << endl;
//                }
//            }
//            cout << "" << endl;
//
//            cout << "point Map " << endl;
//            for (auto const item : pointMap) {
//                for (auto const str : item.second) {
//                    cout << item.first << " " << str.first << endl;
//                }
//            }
//            cout << "" << endl;
//
//            cout << "before route " << endl;
//            for (Route route: (*routeVector)) {
//                cout << "route line : " << route.getStartRowIndx() << " "
//                     << route.getStartColIndx() << " " << route.getStartLayIndx()
//                     << " " << route.getEndRowIndx() << " " << route.getEndColIndx() << " " << route.getEndlayIndx()
//                     << " "
//                     << route.getNetName()
//                     << endl;
//            }
//            cout << "" << endl;


//        map<string, vector<SteinerPoint> > layerSteinerMap
//                map<string, map<string, string>> pointMap;


//    vector<vector<vector<int> > >
//    reviseRouteSupply(vector<vector<vector<int> > > gridVector, vector<Route> numRoute, string revise) {
//
//        for (int i = 0; i < numRoute.size(); i++) {
//            int startLayIndex = numRoute[i].getStartLayIndx();
//            int endLayIndex = numRoute[i].getEndlayIndx();
//            int startRowIndex = numRoute[i].getStartRowIndx();
//            int endRowIndex = numRoute[i].getEndRowIndx();
//            int startColIndex = numRoute[i].getStartColIndx();
//            int endColIndex = numRoute[i].getEndColIndx();
////          cout <<  startRowIndex << " " << startColIndex << " " << startLayIndex << " " << endRowIndex << " " << endColIndex << " "<< endLayIndex << " " <<  numRoute[i].getNetName()<<endl;
//            if (startLayIndex == endLayIndex and startColIndex == endColIndex) {
//                if (startRowIndex > endRowIndex) {
//                    for (int rowIndex = startRowIndex; rowIndex <= endRowIndex; rowIndex++) {
//                        if (revise == REDUCE) {
//                            gridVector[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
//                                    gridVector[startLayIndex - 1][rowIndex - 1][startColIndex - 1] - 1;
//                        } else {
//                            gridVector[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
//                                    gridVector[startLayIndex - 1][rowIndex - 1][startColIndex - 1] + 1;
//                        }
//
//                    }
//                } else {
//                    for (int rowIndex = endRowIndex; rowIndex >= startRowIndex; rowIndex--) {
//                        if (revise == REDUCE) {
//                            gridVector[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
//                                    gridVector[startLayIndex - 1][rowIndex - 1][startColIndex - 1] - 1;
//                        } else {
//                            gridVector[startLayIndex - 1][rowIndex - 1][startColIndex - 1] =
//                                    gridVector[startLayIndex - 1][rowIndex - 1][startColIndex - 1] + 1;
//                        }
//                    }
//                }
//            }
//            if (startLayIndex == endLayIndex and startRowIndex == endRowIndex) {
//                for (int colIndex = startColIndex; colIndex <= endColIndex; colIndex++) {
//                    if (revise == REDUCE) {
//                        gridVector[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
//                                gridVector[startLayIndex - 1][startRowIndex - 1][colIndex - 1] - 1;
//                    } else {
//                        gridVector[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
//                                gridVector[startLayIndex - 1][startRowIndex - 1][colIndex - 1] + 1;
//                    }
//                }
//            } else {
//                for (int colIndex = endColIndex; colIndex >= startColIndex; colIndex--) {
//                    if (revise == REDUCE) {
//                        gridVector[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
//                                gridVector[startLayIndex - 1][startRowIndex - 1][colIndex - 1] - 1;
//                    } else {
//                        gridVector[startLayIndex - 1][startRowIndex - 1][colIndex - 1] =
//                                gridVector[startLayIndex - 1][startRowIndex - 1][colIndex - 1] + 1;
//                    }
//                }
//            }
//        }
//        return gridVector;
//    }


//    void
//    getMinRoutingConstraint(vector<Route> *routeVector, SteinerPoint steinerPoint, string minimumRoutingConstraint,
//                            string netName) {
//        Route route;
//        minimumRoutingConstraint.erase(
//                std::remove(minimumRoutingConstraint.begin(), minimumRoutingConstraint.end(), 'M'),
//                minimumRoutingConstraint.end());
//        route.setStartRowIndx(steinerPoint.getCellPointRow());
//        route.setStartColIndx(steinerPoint.getCellPointCol());
//        route.setEndRowIndx(steinerPoint.getCellPointRow());
//        route.setEndColIndx(steinerPoint.getCellPointCol());
//        route.setStartLayIndx(stoi(minimumRoutingConstraint));
//        route.setEndlayIndx(steinerPoint.getLayer());
//        route.setNetName(netName);
//        (*routeVector).push_back(route);
//    }

//    Route getVia(SteinerPoint lastSteinerPoint, SteinerPoint steinerPoint, string netName, set<string> viaSet) {
//        Route route;
//        int lastSteinerPointRow = lastSteinerPoint.getSteinerPointRow();
//        int lastSteinerPointCol = lastSteinerPoint.getSteinerPointCol();
//        int lastCellRow = lastSteinerPoint.getCellPointRow();
//        int lastCellCol = lastSteinerPoint.getCellPointCol();
//        int cellRow = steinerPoint.getCellPointRow();
//        int cellCol = steinerPoint.getCellPointCol();
//        int steinerPointRow = steinerPoint.getSteinerPointRow();
//        int steinerPointCol = steinerPoint.getSteinerPointCol();
//        if (cellRow == lastCellRow and cellCol == lastCellCol) {
//            route.setNetName(netName);
//            route.setStartRowIndx(lastCellRow);
//            route.setStartColIndx(lastCellCol);
//            route.setEndRowIndx(cellRow);
//            route.setEndColIndx(cellCol);
//            route.setStartLayIndx(lastSteinerPoint.getLayer());
//            route.setEndlayIndx(steinerPoint.getLayer());
//
//        } else if (cellRow == lastSteinerPointRow and cellCol == lastSteinerPointCol) {
//            route.setNetName(netName);
//            route.setStartRowIndx(lastSteinerPointRow);
//            route.setStartColIndx(lastSteinerPointCol);
//            route.setEndRowIndx(cellRow);
//            route.setEndColIndx(cellCol);
//            route.setStartLayIndx(lastSteinerPoint.getLayer());
//            route.setEndlayIndx(steinerPoint.getLayer());
//
//        } else if (steinerPointRow == lastCellRow and steinerPointCol == lastCellCol) {
//            route.setNetName(netName);
//            route.setStartRowIndx(lastCellRow);
//            route.setStartColIndx(lastCellCol);
//            route.setEndRowIndx(steinerPointRow);
//            route.setEndColIndx(steinerPointCol);
//            route.setStartLayIndx(lastSteinerPoint.getLayer());
//            route.setEndlayIndx(steinerPoint.getLayer());
//
//        } else if (steinerPointRow == lastSteinerPointRow and steinerPointCol == lastSteinerPointCol) {
//            route.setNetName(netName);
//            route.setStartRowIndx(lastSteinerPointRow);
//            route.setStartColIndx(lastSteinerPointCol);
//            route.setEndRowIndx(steinerPointRow);
//            route.setEndColIndx(steinerPointCol);
//            route.setStartLayIndx(lastSteinerPoint.getLayer());
//            route.setEndlayIndx(steinerPoint.getLayer());
//
//        } else if (steinerPointRow == lastCellRow and steinerPointCol == lastCellCol) {
//            route.setNetName(netName);
//            route.setStartRowIndx(lastCellRow);
//            route.setStartColIndx(lastCellCol);
//            route.setEndRowIndx(steinerPointRow);
//            route.setEndColIndx(steinerPointCol);
//            route.setStartLayIndx(lastSteinerPoint.getLayer());
//            route.setEndlayIndx(steinerPoint.getLayer());
//        } else {
//            cout << "via has some exception" << endl;
//        }
//
//
//        return route;
//    }

//    string getViaName(Route route) {
//        int startLayIndex = route.getStartLayIndx();
//        int endLayIndex = route.getEndlayIndx();
//        string viaName;
//        if (route.getStartLayIndx() > route.getEndlayIndx()) {
//            viaName = to_string(route.getStartRowIndx()) + "_" + to_string(route.getStartColIndx()) + "_" +
//                      to_string(startLayIndex) + "_" + to_string(endLayIndex);
//        } else {
//            viaName = to_string(route.getStartRowIndx()) + "_" + to_string(route.getStartColIndx()) + "_" +
//                      to_string(endLayIndex) + "_" + to_string(startLayIndex);
//        }
//        return viaName;
//    }

//bool ReRoute::checkDirection(vector<Route> *routeVector) {
//    for (int i = 0; i < (*routeVector).size(); i++) {
//// Horizontal direction
//        if ((*routeVector)[i].getStartRowIndx() == (*routeVector)[i].getEndRowIndx() and
//            (*routeVector)[i].getStartColIndx() != (*routeVector)[i].getEndColIndx() and
//            (*routeVector)[i].getStartLayIndx() == (*routeVector)[i].getEndlayIndx()) {
//            return true;
//// Vertical direction
//        } else if ((*routeVector)[i].getStartRowIndx() != (*routeVector)[i].getEndRowIndx() and
//                   (*routeVector)[i].getStartColIndx() == (*routeVector)[i].getEndColIndx() and
//                   (*routeVector)[i].getStartLayIndx() == (*routeVector)[i].getEndlayIndx()) {
//            return true;
//        } else if ((*routeVector)[i].getStartRowIndx() == (*routeVector)[i].getEndRowIndx() and
//                   (*routeVector)[i].getStartColIndx() == (*routeVector)[i].getEndColIndx() and
//                   (*routeVector)[i].getStartLayIndx() != (*routeVector)[i].getEndlayIndx()) {
//            return true;
//        } else {
//            cout << " wrong direction : " << (*routeVector)[i].getNetName() << endl;
//            cout << (*routeVector)[i].getStartRowIndx() << " " << (*routeVector)[i].getStartColIndx() << " "
//                 << (*routeVector)[i].getStartLayIndx() << " " << (*routeVector)[i].getEndRowIndx() << " "
//                 << (*routeVector)[i].getEndColIndx() << " " << (*routeVector)[i].getEndlayIndx() << " " << endl;
//            return false;
//        }
//// Z- direction
//    }
//}


