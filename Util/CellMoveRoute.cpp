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
#include "../Header/Blockage.h"
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
                                    map<string, vector<int> > *powerFactorMap,
                                    vector<CellInstance> *moveCellInstanceVector, int maxCellMovent,
                                    unordered_map<string, string> *isReRouteMap,
                                    unordered_map<string, VoltageArea> *voltageArearMap) {

    blockageReRoute(&(*netMap), &(*cellInstanceMap), &(*emptyBlockageCellVector), &(*masterCellMap),
                    &(*gridVector), &(*powerFactorMap), &(*moveCellInstanceVector), maxCellMovent, &(*isReRouteMap),
                    &(*voltageArearMap));

}

void CellMoveRoute::blockageReRoute(map<string, Net> *netMap, map<string, CellInstance> *cellInstanceMap,
                                    vector<string> *emptyBlockageCellVector,
                                    map<string, MasterCell> *masterCellMap,
                                    vector<vector<vector<int> > > *gridVector,
                                    map<string, vector<int> > *powerFactorMap,
                                    vector<CellInstance> *moveCellInstanceVector, int maxCellMovent,
                                    unordered_map<string, string> *isReRouteMap,
                                    unordered_map<string, VoltageArea> *voltageArearMap) {
    //所有cell裡面的blockage是空的
    int maxCellMove = 0;
    for (auto const cellInstance:(*cellInstanceMap)) {
        string cell = cellInstance.second.getCellName();
        bool isOnside = true;
        //cell 有哪些net連結到
        if (onOneSide((*cellInstanceMap)[cell], &(*cellInstanceMap), &(*netMap)) and cellInstance.second.getMovalbe() == "Movable") {
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
                (*isReRouteMap).insert(pair<string, string>(connectNet ,connectNet));
            }
            //----------取的所有連接再一起的cell end----------

            //----------取的bounding box start----------
            unordered_map<string, int> boundaryMap;
            vector<CellInstance> movedPosition;
            getBoundaryMap((*cellInstanceMap)[cell], &connectCellMap, &boundaryMap);
            //----------取的bounding box end----------

            //------ 取的moving position start------
            CellInstance oriCellInstance = (*cellInstanceMap)[cell];
            string masterCellName = (*cellInstanceMap)[cell].getMasterCellName();
            MasterCell masterCell = (*masterCellMap)[masterCellName];
            map<string, Blockage> blockageMap = (*masterCellMap)[oriCellInstance.getMasterCellName()].getBlockageType();
            bool canBeMoved = true;
            //所有blockage加回去
//            reviseSupplyCellBlockage(masterCell, &(*gridVector), oriCellInstance, ADD);
            for (int rowIndx = boundaryMap[DOWN]; rowIndx <= boundaryMap[UP]; rowIndx++) {
                for (int colIndx = boundaryMap[LEFT]; colIndx <= boundaryMap[RIGHT]; colIndx++) {
                    if (isOverFlowDemand(&blockageMap, &(*gridVector), rowIndx, colIndx) and isVoltageArea(rowIndx, colIndx, oriCellInstance, &(*voltageArearMap))) {
                        CellInstance moveCellInstance;
                        moveCellInstance.setRowIndx(rowIndx);
                        moveCellInstance.setColIndx(colIndx);
                        moveCellInstance.setCellName(oriCellInstance.getCellName());
                        reviseSupplyCellBlockage(masterCell, &(*gridVector), moveCellInstance, REDUCE);
                        movedCellReRoute(cell, masterCell, moveCellInstance,
                                         oriCellInstance, &(*cellInstanceMap),
                                         &(*netMap), &(*gridVector),
                                         &(*powerFactorMap),
                                         &(*isReRouteMap),
                                         &(*moveCellInstanceVector), &canBeMoved, &maxCellMove);
                        if (canBeMoved) {
                            break;
                        }else{
//                            reviseSupplyCellBlockage(masterCell, &(*gridVector), moveCellInstance, ADD);
                        }
                    }
                }
                if (canBeMoved) {
                    break;
                }
            }

            if(canBeMoved == false){
//                reviseSupplyCellBlockage(masterCell, &(*gridVector), oriCellInstance, REDUCE);
            }

            //------ 取的moving position end------
        }
        if (maxCellMove == maxCellMovent) {
            break;
        }
    }
}


void CellMoveRoute::movedCellReRoute(string movedCellName, MasterCell masterCell, CellInstance moveCellInstance,
                                     CellInstance oriCellInstance, map<string, CellInstance> *cellInstanceMap,
                                     map<string, Net> *netMap, vector<vector<vector<int> > > *gridVector,
                                     map<string, vector<int> > *powerFactorMap,
                                     unordered_map<string, string> *isReRouteMap,
                                     vector<CellInstance> *moveCellInstanceVector, bool *canBeMoved, int *maxCellMove) {
    //------ 兩條net的connectCell 部分改掉改成 moveCellInstance start ------
    ReRoute reRoute;
    for (auto const connectNet : (*cellInstanceMap)[movedCellName].getConnectNetVector()) {
        unordered_map<string, CellInstance> connectCellMap = (*netMap)[connectNet].getConnectCell();
        connectCellMap[movedCellName].setRowIndx(moveCellInstance.getRowIndx());
        connectCellMap[movedCellName].setColIndx(moveCellInstance.getColIndx());
        (*netMap)[connectNet].setConnectCell(connectCellMap);
    }
    //------ 兩條net的connectCell 部分改掉改成 moveCellInstance end ------
    unordered_map<string, vector<Route> > oriRouteMap;
    //---------- 先拔掉所有的繞線，再將route 暫存在在外面 start----------
    for (auto const connectNet : (*cellInstanceMap)[movedCellName].getConnectNetVector()) {
        vector<Route> numRoute = (*netMap)[connectNet].getNumRoute();
        oriRouteMap.insert(pair<string, vector<Route> >(connectNet, numRoute));
        reRoute.reviseRouteSupply(&(*gridVector), &numRoute, ADD, connectNet);
    }
    //---------- 先拔掉所有的繞線，再將oriroute 暫存在在外面 end----------
    //---------- 先將Cell blockage 加回去 start ----------
    reviseSupplyCellBlockage(masterCell, &(*gridVector), oriCellInstance, ADD);
    //---------- 先將Cell blockage 加回去 end ----------
    //---------- 將新的Cell blockage 減去 start ----------
    reviseSupplyCellBlockage(masterCell, &(*gridVector), moveCellInstance, REDUCE);
    //---------- 先將Cell blockage 減去 end ----------
    unordered_map<string, vector<Route> > successRouteMap;

    for (auto const connectNet : (*cellInstanceMap)[movedCellName].getConnectNetVector()) {
        vector<Route> oriRoute = (*netMap)[connectNet].getNumRoute();
        vector<Route> routeVector;
        reRoute.getSteinerRoute(&routeVector, connectNet, &(*netMap), &(*gridVector),
                                &(*powerFactorMap));
        int afterRouteLength = caculateRouteLength(&routeVector);
        int oriRouteLength;
        if((*netMap)[connectNet].getRouteLength() == 0){
            oriRouteLength = caculateRouteLength(&oriRoute);
            (*netMap)[connectNet].setRouteLength(oriRouteLength);
        }else{
            oriRouteLength = (*netMap)[connectNet].getRouteLength();
        }
        if (routeVector.size() > 0 and afterRouteLength <= oriRouteLength) {
            reRoute.reviseRouteSupply(&(*gridVector), &routeVector, REDUCE, connectNet);
            successRouteMap.insert(pair<string, vector<Route> >(connectNet, routeVector));
            (*canBeMoved) = true;
        } else {
            (*canBeMoved) = false;
            break;
        }
    }
    // 失敗處理
    if ((*canBeMoved) == false) {
        for (const auto routeVec:successRouteMap) {
            vector<Route> routeVector = routeVec.second;
            reRoute.reviseRouteSupply(&(*gridVector), &routeVector, ADD, routeVec.first);
        }
        for (const auto routeVec:oriRouteMap) {
            vector<Route> routeVector = routeVec.second;
            reRoute.reviseRouteSupply(&(*gridVector), &routeVector, REDUCE, routeVec.first);
        }
        for (auto const connectNet : (*cellInstanceMap)[movedCellName].getConnectNetVector()) {
            unordered_map<string, CellInstance> connectCellMap = (*netMap)[connectNet].getConnectCell();
            connectCellMap[movedCellName].setRowIndx(oriCellInstance.getRowIndx());
            connectCellMap[movedCellName].setColIndx(oriCellInstance.getColIndx());
            (*netMap)[connectNet].setConnectCell(connectCellMap);
        }
        reviseSupplyCellBlockage(masterCell, &(*gridVector), moveCellInstance, ADD);
        reviseSupplyCellBlockage(masterCell, &(*gridVector), oriCellInstance, REDUCE);
        //成功處理
    } else {
        for (const auto routeVec:successRouteMap) {
            vector<Route> routeVector = routeVec.second;
            (*netMap)[routeVec.first].setNumRoute(routeVector);
        }
        (*cellInstanceMap)[movedCellName].setRowIndx(moveCellInstance.getRowIndx());
        (*cellInstanceMap)[movedCellName].setColIndx(moveCellInstance.getColIndx());
        //儲存移動的map
        (*moveCellInstanceVector).push_back(moveCellInstance);
        (*maxCellMove) += 1;
    }
}


int CellMoveRoute::caculateRouteLength(vector<Route> *routeVector) {
    int totalLength = 0;
    for (int i = 0; i < (*routeVector).size(); i++) {
        int startRowIndx = (*routeVector)[i].getStartRowIndx();
        int startColIndx = (*routeVector)[i].getStartColIndx();
        int endRowIndx = (*routeVector)[i].getEndRowIndx();
        int endColIndx = (*routeVector)[i].getEndColIndx();
        int startLayIndx = (*routeVector)[i].getStartLayIndx();
        int endLayIndx = (*routeVector)[i].getEndlayIndx();
        if (startRowIndx == endRowIndx and startColIndx != endColIndx) {
            int distance = startColIndx - endColIndx;
            distance = abs(distance);
            totalLength += distance;
            totalLength += 1;
        } else if (startRowIndx != endRowIndx and startColIndx == endColIndx) {
            int distance = startRowIndx - endRowIndx;
            distance = abs(distance);
            totalLength += distance;
            totalLength += 1;
        } else if (startLayIndx != endLayIndx and startRowIndx == endRowIndx and startColIndx == endColIndx) {
            int distance = startLayIndx - endLayIndx;
            distance = abs(distance);
            totalLength += distance;
        } else {
            cout << "has some exception in route" << endl;
        }

    }
    return totalLength;
}


void CellMoveRoute::reviseSupplyCellBlockage(MasterCell masterCell, vector<vector<vector<int> > > *gridVector,
                                             CellInstance cellInstance, string revise) {
    map<string, Blockage> blockageMap = masterCell.getBlockageType();
    for (auto const &item : blockageMap) {
        if (revise == REDUCE) {
            (*gridVector)[item.second.getBlockageLayer() - 1][cellInstance.getRowIndx() - 1][
                    cellInstance.getColIndx() - 1] =
                    (*gridVector)[item.second.getBlockageLayer() - 1][cellInstance.getRowIndx() - 1][
                            cellInstance.getColIndx() - 1] - item.second.getDemand();

        } else {
            (*gridVector)[item.second.getBlockageLayer() - 1][cellInstance.getRowIndx() - 1][
                    cellInstance.getColIndx() - 1] =
                    (*gridVector)[item.second.getBlockageLayer() - 1][cellInstance.getRowIndx() - 1][
                            cellInstance.getColIndx() - 1] + item.second.getDemand();

        }
    }
}


bool CellMoveRoute::isVoltageArea(int rowIndx, int colIndx, CellInstance cellInstance,
                                  unordered_map<string, VoltageArea> *voltageArearMap) {
    string cellName = cellInstance.getCellName();
    string movedPosition = to_string(rowIndx) + "_" + to_string(colIndx);
    if ((*voltageArearMap).find(cellName) == (*voltageArearMap).end()) {
        return true;
    } else {
        unordered_map<string, string> gridMap = (*voltageArearMap)[cellName].getGridMap();
        if (gridMap.find(movedPosition) == gridMap.end()) {
            return false;
        } else {
            return true;
        }
    }
    return true;
}


bool CellMoveRoute::isOverFlowDemand(map<string, Blockage> *blockageMap, vector<vector<vector<int> > > *gridVector,
                                     int rowIndx, int colIndx) {
    bool isValid = true;
    for (auto const &item : *blockageMap) {
        //row col 相反搞錯位置
        int gridDemand =
                (*gridVector)[item.second.getBlockageLayer() - 1][rowIndx - 1][colIndx - 1] - item.second.getDemand();
        if (gridDemand <= 0) {
            isValid = false;
            break;
        }
    }
    return isValid;
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
bool CellMoveRoute::onOneSide(CellInstance cell, map<string, CellInstance> *cellInstanceMap, map<string, Net> *netMap) {
    set<string> sideSet;
    bool isOnOneSide = false;
    bool isOnSameLine = false;
    for (auto const connectNet : (*cellInstanceMap)[cell.getCellName()].getConnectNetVector()) {
        for (auto const connectCell: (*netMap)[connectNet].getConnectCell()) {
            if (cell.getRowIndx() == connectCell.second.getRowIndx() and cell.getColIndx() != connectCell.second.getColIndx()) {
                if (cell.getColIndx() < connectCell.second.getColIndx()) {
                    sideSet.insert(RIGHTPOINT);
                } else {
                    sideSet.insert(LEFTPOINT);
                }
            } else if (cell.getRowIndx() != connectCell.second.getRowIndx() and cell.getColIndx() == connectCell.second.getColIndx()) {
                if (cell.getRowIndx() < connectCell.second.getRowIndx()) {
                    sideSet.insert(UPPOINT);
                } else {
                    sideSet.insert(DOWNPOINT);
                }
            } else if (cell.getRowIndx() != connectCell.second.getRowIndx() and cell.getColIndx() != connectCell.second.getColIndx()) {
                //右上
                if (cell.getRowIndx() < connectCell.second.getRowIndx() and cell.getColIndx() < connectCell.second.getColIndx()) {
                    sideSet.insert(UPPOINT);
                    sideSet.insert(RIGHTPOINT);
                    //右下
                } else if (cell.getRowIndx() > connectCell.second.getRowIndx() and cell.getColIndx() < connectCell.second.getColIndx()) {
                    sideSet.insert(DOWNPOINT);
                    sideSet.insert(RIGHTPOINT);
                    //左上
                } else if (cell.getRowIndx() > connectCell.second.getRowIndx() and cell.getColIndx() > connectCell.second.getColIndx()) {
                    sideSet.insert(UPPOINT);
                    sideSet.insert(LEFTPOINT);
                    //左下
                } else if (cell.getRowIndx() < connectCell.second.getRowIndx() and cell.getColIndx() > connectCell.second.getColIndx()) {
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

//void CellMoveRoute::getCellMovePosition(string movedCell, set<string> *movedPositionVector,
//                                        map<string, CellInstance> *cellInstanceMap, map<string, Net> *netMap) {
//    vector<string> connectNetVec = (*cellInstanceMap)[movedCell].getConnectNetVector();
//    for (int i = 0; i < connectNetVec.size(); i++) {
//        unordered_map<string, CellInstance> connectCellMap = (*netMap)[connectNetVec[i]].getConnectCell();
//        for (const auto cellInstance :  connectCellMap) {
//            if (cellInstance.second.getCellName() != movedCell) {
//                (*movedPositionVector).insert(cellInstance.second.getCellName());
//            }
//        }
//    }
//}


//重新繞線
//void CellMoveRoute::reRouteNet(string connectNet, map<string, Net> *netMap, map<string, CellInstance> *cellInstanceMap,
//                               map<string, vector<int> > *powerFactorMap, vector<vector<vector<int> > > *gridVector) {
//    ReRoute reRoute;
//    vector<Route> numRoute = (*netMap)[connectNet].getNumRoute();
//    reRoute.reviseRouteSupply(&(*gridVector), &numRoute, ADD, connectNet);
//    vector<Route> routeVector;
//    reRoute.getSteinerRoute(&routeVector, connectNet, &(*netMap), &(*gridVector), &(*powerFactorMap));
//    if (routeVector.size() > 0) {
//        (*netMap)[connectNet].setNumRoute(routeVector);
//        //減掉新的線段
//        reRoute.reviseRouteSupply(&(*gridVector), &routeVector, REDUCE, connectNet);
//    } else {
//        //減掉原來的線段
//        reRoute.reviseRouteSupply(&(*gridVector), &numRoute, REDUCE, connectNet);
//    }
//
//}






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

//void CellMoveRoute::movedPoint(unordered_map<string, int> *boundaryMap, vector<CellInstance> *movePosition,
//                               CellInstance oriCellInstance, map<string, MasterCell> *masterCellMap,
//                               vector<vector<vector<int> > > *gridVector,
//                               unordered_map<string, VoltageArea> *voltageArearMap) {
//
//    string masterCellName = oriCellInstance.getMasterCellName();
//    map<string, Blockage> blockageMap = (*masterCellMap)[masterCellName].getBlockageType();
//    for (int rowIndx = (*boundaryMap)[DOWN]; rowIndx <= (*boundaryMap)[UP]; rowIndx++) {
//        for (int colIndx = (*boundaryMap)[LEFT]; colIndx <= (*boundaryMap)[RIGHT]; colIndx++) {
//            if (isOverFlowDemand(&blockageMap, &(*gridVector), rowIndx, colIndx) and
//                isVoltageArea(rowIndx, colIndx, oriCellInstance, &(*voltageArearMap))) {
//                CellInstance movedCell;
//                movedCell.setRowIndx(rowIndx);
//                movedCell.setColIndx(colIndx);
//                movedCell.setCellName(oriCellInstance.getCellName());
//                (*movePosition).push_back(movedCell);
//            }
//        }
//    }
//
//}
