//
// Created by Ｗilson on 2021/8/9.
//

#include "CellMoveRoute.h"
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */
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

CellMoveRoute::CellMoveRoute() {}

CellMoveRoute::~CellMoveRoute() {

}


void CellMoveRoute::cellMoveReRoute(map<string, Net> *netMap, map<string, CellInstance> *cellInstanceMap,
                                    vector<string> *emptyBlockageCellVector, map<string, MasterCell> *masterCellMap,
                                    vector<vector<vector<int> > > *gridVector,
                                    map<string, vector<int> > *powerFactorMap,vector<CellInstance> *moveCellInstanceVector,int maxCellMovent, unordered_map<string,string> *isReRouteMap) {

    emptyBlockageReRoute(&(*netMap), &(*cellInstanceMap), &(*emptyBlockageCellVector), &(*masterCellMap),
                         &(*gridVector), &(*powerFactorMap), &(*moveCellInstanceVector), maxCellMovent, &(*isReRouteMap));

}

void CellMoveRoute::emptyBlockageReRoute(map<string, Net> *netMap, map<string, CellInstance> *cellInstanceMap,
                                         vector<string> *emptyBlockageCellVector,
                                         map<string, MasterCell> *masterCellMap,
                                         vector<vector<vector<int> > > *gridVector,
                                         map<string, vector<int> > *powerFactorMap,
                                         vector<CellInstance> *moveCellInstanceVector, int maxCellMovent, unordered_map<string,string> *isReRouteMap) {
    //所有cell裡面的blockage是空的
    int maxCellMove = 0;
    for (auto const cell:(*emptyBlockageCellVector)) {
        bool isOnside = true;
        //cell 有哪些net連結到
        if (onOneSide((*cellInstanceMap)[cell], &(*cellInstanceMap), &(*netMap))) {
            isOnside = true;
        } else {
            isOnside = false;
        }
        //全部都在同一側開始做CellMove
        if (isOnside) {
            cout << "need Cell Move : " << cell << endl;
            //step 1 : 減掉跟這條cell一樣所有的繞線，加上supply 跟 grid
            //step 2 : 取得繞線
            ReRoute reRoute;
            unordered_map<string, CellInstance> connectCellMap;
            //----------取的所有連接再一起的cell start----------
            for (auto const connectNet : (*cellInstanceMap)[cell].getConnectNetVector()) {
                unordered_map<string, CellInstance> connectMap = (*netMap)[connectNet].getConnectCell();
                connectCellMap.insert(connectMap.begin(), connectMap.end());
            }
            //----------取的所有連接再一起的cell end----------

            //----------取的bounding box start----------
            unordered_map<string, int> boundaryMap;
            CellInstance moveCellInstance;
            getBoundaryMap((*cellInstanceMap)[cell], &connectCellMap, &boundaryMap);
            //----------取的bounding box end----------

            //------ 取的moving position start------
            srand(time(NULL));
            CellInstance oriCellInstance = (*cellInstanceMap)[cell];
            set<string> cellMoveSet;
            for (int i = 0; i < 3; i++) {
                randomMovingPoint(boundaryMap, &moveCellInstance, cell);
                if (isRepeatCellMove(moveCellInstance, &cellMoveSet) == false) {
                    //------ 兩條net的connectCell 部分改掉改成 moveCellInstance start ------
                    for (auto const connectNet : (*cellInstanceMap)[cell].getConnectNetVector()) {
                        unordered_map<string, CellInstance> connectCellMap = (*netMap)[connectNet].getConnectCell();
                        connectCellMap[cell].setRowIndx(moveCellInstance.getRowIndx());
                        connectCellMap[cell].setColIndx(moveCellInstance.getColIndx());
                        (*netMap)[connectNet].setConnectCell(connectCellMap);
                    }
                    //------ 兩條net的connectCell 部分改掉改成 moveCellInstance end ------
                    unordered_map<string, vector<Route> > oriRouteMap;
                    //---------- 先拔掉所有的繞線，再將route 暫存在在外面 start----------
                    for (auto const connectNet : (*cellInstanceMap)[cell].getConnectNetVector()) {
                        vector<Route> numRoute = (*netMap)[connectNet].getNumRoute();
                        oriRouteMap.insert(pair<string, vector<Route> >(connectNet, numRoute));
                        reRoute.reviseRouteSupply(&(*gridVector), &numRoute, ADD, connectNet);
                    }
                    //---------- 先拔掉所有的繞線，再將oriroute 暫存在在外面 end----------

                    bool canBeMoved = true;
                    unordered_map<string, vector<Route> > successRouteMap;
                    for (auto const connectNet : (*cellInstanceMap)[cell].getConnectNetVector()) {
                        vector<Route> routeVector;
                        reRoute.getSteinerRoute(&routeVector, connectNet, &(*netMap), &(*gridVector),
                                                &(*powerFactorMap));

                        if (routeVector.size() > 0) {
                            reRoute.reviseRouteSupply(&(*gridVector), &routeVector, REDUCE, connectNet);
                            successRouteMap.insert(pair<string, vector<Route> >(connectNet, routeVector));
                        } else {
                            canBeMoved = false;
                            break;
                        }
                    }
                    // 失敗處理
                    if (canBeMoved == false) {
                        for (const auto routeVec:successRouteMap) {
                            vector<Route> routeVector = routeVec.second;
                            reRoute.reviseRouteSupply(&(*gridVector), &routeVector, ADD, routeVec.first);
                        }
                        for (const auto routeVec:oriRouteMap) {
                            vector<Route> routeVector = routeVec.second;
                            reRoute.reviseRouteSupply(&(*gridVector), &routeVector, REDUCE, routeVec.first);
                        }
                        for (auto const connectNet : (*cellInstanceMap)[cell].getConnectNetVector()) {
                            unordered_map<string, CellInstance> connectCellMap = (*netMap)[connectNet].getConnectCell();
                            connectCellMap[cell].setRowIndx(oriCellInstance.getRowIndx());
                            connectCellMap[cell].setColIndx(oriCellInstance.getColIndx());
                            (*netMap)[connectNet].setConnectCell(connectCellMap);
                        }
                        //成功處理
                    } else {
                        for (const auto routeVec:successRouteMap) {
                            vector<Route> routeVector = routeVec.second;
                            (*netMap)[routeVec.first].setNumRoute(routeVector);
                            (*isReRouteMap).insert(pair<string,string>(routeVec.first,routeVec.first));
                        }
                        (*cellInstanceMap)[cell].setRowIndx(moveCellInstance.getRowIndx());
                        (*cellInstanceMap)[cell].setColIndx(moveCellInstance.getColIndx());
                        //儲存移動的map
                        (*moveCellInstanceVector).push_back(moveCellInstance);
                        maxCellMove +=1;
                        break;
                    }
                }
            };
            //------ 取的moving position end------
        }
        if(maxCellMove == maxCellMovent){
            break;
        }
    }
}

bool CellMoveRoute::isRepeatCellMove(CellInstance moveCellInstance, set<string> *cellMoveSet) {
    string cell_move = to_string(moveCellInstance.getRowIndx()) + "_" + to_string(moveCellInstance.getColIndx());
    int oriSize = (*cellMoveSet).size();
    (*cellMoveSet).insert(cell_move);
    int afterSize = (*cellMoveSet).size();

    if (oriSize == afterSize) {
        return true;
    } else {
        return false;
    }
}

void CellMoveRoute::randomMovingPoint(unordered_map<string, int> boundaryMap, CellInstance *moveCellInstance,string cellName) {
    int rowRange = boundaryMap[RIGHT] - boundaryMap[LEFT];
    int row = rand() % rowRange + boundaryMap[LEFT];
    int colRange = boundaryMap[UP] - boundaryMap[DOWN];
    int col = rand() % colRange + boundaryMap[DOWN];
    (*moveCellInstance).setRowIndx(row);
    (*moveCellInstance).setColIndx(col);
    (*moveCellInstance).setCellName(cellName);
}

void CellMoveRoute::getBoundaryMap(CellInstance movedCell, unordered_map<string, CellInstance> *connectCellMap,
                                   unordered_map<string, int> *boundaryMap) {
    for (unordered_map<string, CellInstance>::iterator cell_map_it = (*connectCellMap).begin();
         cell_map_it != (*connectCellMap).end(); ++cell_map_it) {
        CellInstance cellInstance = cell_map_it->second;
        if (cellInstance.getCellName() != movedCell.getCellName()) {
            getBoundaryBox(cellInstance, &(*boundaryMap));
        }
    }


};


void CellMoveRoute::getBoundaryBox(CellInstance cellInstance, unordered_map<string, int> *boundaryMap) {

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

void CellMoveRoute::getCellMovePosition(string movedCell, set<string> *movedPositionVector,
                                        map<string, CellInstance> *cellInstanceMap, map<string, Net> *netMap) {
    vector<string> connectNetVec = (*cellInstanceMap)[movedCell].getConnectNetVector();
    for (int i = 0; i < connectNetVec.size(); i++) {
        unordered_map<string, CellInstance> connectCellMap = (*netMap)[connectNetVec[i]].getConnectCell();
        for (const auto cellInstance :  connectCellMap) {
            if (cellInstance.second.getCellName() != movedCell) {
                (*movedPositionVector).insert(cellInstance.second.getCellName());
            }
        }
    }
}

void CellMoveRoute::getRandomCellVector(set<string> *cellMovePositionSet, vector<string> *cellMovePositionVector) {
    vector<string> cellMoveVector((*cellMovePositionSet).begin(), (*cellMovePositionSet).end());
    int randomSize = cellMoveVector.size() / 3;
    for (int i = 0; i < randomSize; i++) {
        srand(time(NULL));
        int random = rand() % (cellMoveVector.size() - 1);
        (*cellMovePositionVector).push_back(cellMoveVector[random]);
        cellMoveVector.erase(cellMoveVector.begin() + random);
    }
}


//重新繞線
void CellMoveRoute::reRouteNet(string connectNet, map<string, Net> *netMap, map<string, CellInstance> *cellInstanceMap,
                               map<string, vector<int> > *powerFactorMap, vector<vector<vector<int> > > *gridVector) {
    ReRoute reRoute;
    vector<Route> numRoute = (*netMap)[connectNet].getNumRoute();
    reRoute.reviseRouteSupply(&(*gridVector), &numRoute, ADD, connectNet);
    vector<Route> routeVector;
    reRoute.getSteinerRoute(&routeVector, connectNet, &(*netMap), &(*gridVector), &(*powerFactorMap));
    if (routeVector.size() > 0) {
        (*netMap)[connectNet].setNumRoute(routeVector);
        //減掉新的線段
        reRoute.reviseRouteSupply(&(*gridVector), &routeVector, REDUCE, connectNet);
    } else {
        //減掉原來的線段
        reRoute.reviseRouteSupply(&(*gridVector), &numRoute, REDUCE, connectNet);
    }

}


bool CellMoveRoute::onOneSide(CellInstance cell, map<string, CellInstance> *cellInstanceMap, map<string, Net> *netMap) {
    set<string> sideSet;
    bool isOnOneSide = false;
    for (auto const connectNet : (*cellInstanceMap)[cell.getCellName()].getConnectNetVector()) {
        for (auto const connectCell: (*netMap)[connectNet].getConnectCell()) {
            if (cell.getRowIndx() == connectCell.second.getRowIndx() and
                cell.getColIndx() != connectCell.second.getColIndx()) {
                if (cell.getColIndx() < connectCell.second.getColIndx()) {
                    sideSet.insert(RIGHTPOINT);
                } else {
                    sideSet.insert(LEFTPOINT);
                }
            } else if (cell.getRowIndx() != connectCell.second.getRowIndx() and
                       cell.getColIndx() == connectCell.second.getColIndx()) {
                if (cell.getRowIndx() < connectCell.second.getRowIndx()) {
                    sideSet.insert(UPPOINT);
                } else {
                    sideSet.insert(DOWNPOINT);
                }
            } else if (cell.getRowIndx() != connectCell.second.getRowIndx() and
                       cell.getColIndx() != connectCell.second.getColIndx()) {
                //右上
                if (cell.getRowIndx() < connectCell.second.getRowIndx() and
                    cell.getColIndx() < connectCell.second.getColIndx()) {
                    sideSet.insert(UPPOINT);
                    sideSet.insert(RIGHTPOINT);
                    //右下
                } else if (cell.getRowIndx() > connectCell.second.getRowIndx() and
                           cell.getColIndx() < connectCell.second.getColIndx()) {
                    sideSet.insert(DOWNPOINT);
                    sideSet.insert(RIGHTPOINT);
                    //左上
                } else if (cell.getRowIndx() > connectCell.second.getRowIndx() and
                           cell.getColIndx() > connectCell.second.getColIndx()) {
                    sideSet.insert(UPPOINT);
                    sideSet.insert(LEFTPOINT);
                    //左下
                } else if (cell.getRowIndx() < connectCell.second.getRowIndx() and
                           cell.getColIndx() > connectCell.second.getColIndx()) {
                    sideSet.insert(DOWNPOINT);
                    sideSet.insert(LEFTPOINT);
                } else {
                    cout << "has check exception " << endl;
                    cout << "cell : " << cell.getCellName() << " " << cell.getRowIndx() << " " << cell.getColIndx()
                         << endl;
                    cout << "connectCell : " << connectCell.second.getCellName() << " "
                         << connectCell.second.getRowIndx() << " " << connectCell.second.getColIndx() << endl;
                }
            }
        };
        if (sideSet.size() > 3) {
            isOnOneSide = false;
            break;
        } else {
            isOnOneSide = true;
        }
    };
    return isOnOneSide;
}





//----------拔掉所有的線段----------
//for (auto const connectNet : (*cellInstanceMap)[cell].getConnectNetVector()) {
//vector<Route> numRoute = (*netMap)[connectNet].getNumRoute();
//reRoute.reviseRouteSupply(&(*gridVector), &numRoute, ADD, connectNet);
//}
//----------拔掉所有的線段----------
//----------取得所有連接的cell 並從其中挑選每條線段的cell個數的5%作為cell移動到同一個上面 ----------


//----------取得所有連接的cell  並從其中挑選線段的5%作為cell移動到同一個上面----------
//----------for each 所有 cell 並且netMap 及 cellInstanceMap裡面的cell要記得改位置 變動後成功失敗變動皆要改 ----------
//master cell 不用改

//----------for each 所有 cell 並且netMap 及 cellInstanceMap裡面的cell要記得改位置 變動後成功失敗變動皆要改 ---------



//            //Cell move
//            vector<RoutePoint> routePointVector;
//            getRoutePointVector( connectNet, &(*netMap), &(*cellInstanceMap),&routePointVector);
//            //如何移動cell 移動完失敗後 要再移回去
//
//
//
//            // ----- reRoute Net start ------
//            vector<Route> numRoute = (*netMap)[connectNet].getNumRoute();
//            reRoute.reviseRouteSupply(&(*gridVector), &numRoute, ADD, connectNet);
//            vector<Route> routeVector;
//            reRoute.getSteinerRoute(&routeVector, connectNet, &(*netMap), &(*gridVector), &(*powerFactorMap));
//            if (routeVector.size() > 0) {
//                (*netMap)[connectNet].setNumRoute(routeVector);
//                //減掉新的線段
//                reRoute.reviseRouteSupply(&(*gridVector), &routeVector, REDUCE, connectNet);
//            } else {
//                //減掉原來的線段
//                reRoute.reviseRouteSupply(&(*gridVector), &numRoute, REDUCE, connectNet);
//            }

//   三角形用
//    string closePoint = UP;
//    int longerdistance = 0;
//    for(const auto boundryPoint : boundaryMap){
//        cout << "boundary : " <<boundryPoint.second.getCellName() << endl;
//        if(boundryPoint.first == UP or boundryPoint.first == DOWN){
//            int distance = boundaryMap[boundryPoint.first].getRowIndx() - movedCell.getRowIndx();
//            distance = abs(distance);
//            cout << "distance : " << distance << endl;
//         if(distance<=longerdistance){
//             longerdistance = distance;
//             closePoint = boundryPoint.first;
//
//         }
//        }else{
//            int distance = boundaryMap[boundryPoint.first].getColIndx() - movedCell.getColIndx();
//            distance = abs(distance);
//            cout << "distance : " << distance << endl;
//            if(distance<=longerdistance){
//                longerdistance = distance;
//                closePoint = boundryPoint.first;
//            }
//        }
//    }
//    for(const auto boundryPoint : boundaryMap){
//         if(boundryPoint.first != closePoint){
//             (*trianglePoint).push_back(boundryPoint.second);
//         }
//    }

