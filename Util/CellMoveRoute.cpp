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


void CellMoveRoute::cellMoveReRoute(map<string, Net> *netMap,map<string, CellInstance> *cellInstanceMap,vector<string> *emptyBlockageCellVector,map<string, MasterCell> *masterCellMap,vector<vector<vector<int> > > *gridVector,map<string, vector<int> > *powerFactorMap) {
    emptyBlockageReRoute(&(*netMap),&(*cellInstanceMap), &(*emptyBlockageCellVector), &(*masterCellMap),&(*gridVector), &(*powerFactorMap));

}

void CellMoveRoute::emptyBlockageReRoute(map<string, Net> *netMap,map<string, CellInstance> *cellInstanceMap,vector<string> *emptyBlockageCellVector,map<string, MasterCell> *masterCellMap,vector<vector<vector<int> > > *gridVector,map<string, vector<int> > *powerFactorMap) {
    //所有cell裡面的blockage是空的
    for (auto const cell:(*emptyBlockageCellVector)) {
        bool isOnside = true;
        //cell 有哪些net連結到
        if(onOneSide((*cellInstanceMap)[cell], &(*cellInstanceMap),  &(*netMap))){
            isOnside = true;
        }else{
            isOnside = false;
        }
        //全部都在同一側開始做CellMove
        if (isOnside) {
            cout << "need Cell Move : " << cell << endl;
            //step 1 : 減掉跟這條cell一樣所有的繞線，加上supply 跟 grid
            //step 2 : 取得繞線
            ReRoute reRoute;
            vector<CellInstance> connectCellVector;
            //----------拔掉所有的線段 start----------
            for (auto const connectNet : (*cellInstanceMap)[cell].getConnectNetVector()) {
                    vector<Route> numRoute = (*netMap)[connectNet].getNumRoute();
                    vector<CellInstance> cellVector=  (*netMap)[connectNet].getConnectCell();
                    connectCellVector.insert(connectCellVector.end(),cellVector.begin(),cellVector.end());
                    reRoute.reviseRouteSupply(&(*gridVector), &numRoute, ADD, connectNet);
            }
            //----------拔掉所有的線段 end----------
            //----------取得bounding box 其他三點 start----------
            for(int i =0;i<connectCellVector.size();i++){
                cout << "cell move : " <<connectCellVector[i].getCellName() << endl;
            }
            unordered_map<string,int> boundaryMap;
            CellInstance moveCellInstance;
            getBoundaryMap((*cellInstanceMap)[cell],&connectCellVector, &boundaryMap);
            for(auto const point : boundaryMap){
                cout << point.first << " " << point.second << endl;
            }
            srand(time(NULL));
            for(int i =0;i < 3 ;i++){
                randomMovingPoint(boundaryMap, &moveCellInstance);
                


            };









        }
    }
}

void CellMoveRoute:: randomMovingPoint(unordered_map<string,int> boundaryMap,CellInstance *moveCellInstance){
    int rowRange = boundaryMap[RIGHT] - boundaryMap[LEFT];
    int row=rand()%rowRange + boundaryMap[LEFT];
    int colRange = boundaryMap[UP] - boundaryMap[DOWN];
    int col=rand()%colRange + boundaryMap[DOWN];
    (*moveCellInstance).setRowIndx(row);
    (*moveCellInstance).setColIndx(col);
}

void CellMoveRoute:: getBoundaryMap(CellInstance movedCell,vector<CellInstance> *cellInstanceVector,unordered_map<string,int> *boundaryMap){
    for(int i =0;i < (*cellInstanceVector).size();i++){
        CellInstance cellInstance = (*cellInstanceVector)[i];
        if(cellInstance.getCellName() != movedCell.getCellName()){
            cout << cellInstance.getCellName() << " " << cellInstance.getRowIndx() << " " << cellInstance.getColIndx() << endl;
            getBoundaryBox(cellInstance, &(*boundaryMap));
        }
    }
};


void CellMoveRoute::getBoundaryBox(CellInstance cellInstance,unordered_map<string,int> *boundaryMap){

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

void CellMoveRoute::getCellMovePosition(string movedCell,set<string> *movedPositionVector,map<string, CellInstance> *cellInstanceMap,map<string, Net> *netMap){
        vector<string> connectNetVec =  (*cellInstanceMap)[movedCell].getConnectNetVector();
        cout << "movedCell : " << movedCell << endl;
        for(int i=0;i< connectNetVec.size();i++){
            vector<CellInstance> connectCellVec =  (*netMap)[connectNetVec[i]].getConnectCell();
            for(const auto cellInstance :  connectCellVec){
                if(cellInstance.getCellName()!=movedCell){
                    (*movedPositionVector).insert(cellInstance.getCellName());
                }
            }
        }
}

void CellMoveRoute::getRandomCellVector(set<string> *cellMovePositionSet,vector<string> *cellMovePositionVector){
    vector<string> cellMoveVector((*cellMovePositionSet).begin(), (*cellMovePositionSet).end());
    int randomSize = cellMoveVector.size()/3;
    for(int i =0 ;i < randomSize;i++){
        srand(time(NULL));
        int random=rand()%(cellMoveVector.size()-1);
        (*cellMovePositionVector).push_back(cellMoveVector[random]);
        cellMoveVector.erase( cellMoveVector.begin() + random );
    }
}


void CellMoveRoute::getRoutePointVector(string reRouteNet,map<string, Net> *netMap,map<string, CellInstance> *cellInstanceMap,vector<RoutePoint> *routePointVector){
        vector<CellInstance> cellInstanceVector = (*netMap)[reRouteNet].getConnectCell();
       for(int i = 0; i < (cellInstanceVector.size()-1);i++){
            RoutePoint routePoint;
            routePoint.setStartPointRow(cellInstanceVector[i].getRowIndx());
            routePoint.setStartPointCol(cellInstanceVector[i].getColIndx());
            routePoint.setEndPointRow(cellInstanceVector[i+1].getRowIndx());
            routePoint.setEndPointCol(cellInstanceVector[i+1].getColIndx());
           (*routePointVector).push_back(routePoint);
       };
}

//重新繞線
void CellMoveRoute::reRouteNet(string connectNet,map<string, Net> *netMap,map<string, CellInstance> *cellInstanceMap,map<string, vector<int> > *powerFactorMap,vector<vector<vector<int> > > *gridVector){
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



bool CellMoveRoute::onOneSide(CellInstance cell, map<string, CellInstance> *cellInstanceMap, map<string, Net> *netMap)
{
    set<string> sideSet;
    bool isOnOneSide = false;
    for (auto const connectNet : (*cellInstanceMap)[cell.getCellName()].getConnectNetVector()) {
        for (auto const connectCell: (*netMap)[connectNet].getConnectCell()) {
            if (cell.getRowIndx() == connectCell.getRowIndx() and cell.getColIndx() != connectCell.getColIndx()) {
                if (cell.getColIndx() < connectCell.getColIndx()) {
                    sideSet.insert(RIGHTPOINT);
                } else {
                    sideSet.insert(LEFTPOINT);
                }
            } else if (cell.getRowIndx() != connectCell.getRowIndx() and cell.getColIndx() == connectCell.getColIndx()) {
                if (cell.getRowIndx() < connectCell.getRowIndx()) {
                    sideSet.insert(UPPOINT);
                } else {
                    sideSet.insert(DOWNPOINT);
                }
            } else if (cell.getRowIndx() != connectCell.getRowIndx() and cell.getColIndx() != connectCell.getColIndx()) {
                //右上
                if (cell.getRowIndx() < connectCell.getRowIndx() and cell.getColIndx() < connectCell.getColIndx()) {
                    sideSet.insert(UPPOINT);
                    sideSet.insert(RIGHTPOINT);
                    //右下
                } else if (cell.getRowIndx() > connectCell.getRowIndx() and cell.getColIndx() < connectCell.getColIndx()){
                    sideSet.insert(DOWNPOINT);
                    sideSet.insert(RIGHTPOINT);
                    //左上
                }else if(cell.getRowIndx() > connectCell.getRowIndx() and cell.getColIndx() > connectCell.getColIndx()){
                    sideSet.insert(UPPOINT);
                    sideSet.insert(LEFTPOINT);
                    //左下
                }else if(cell.getRowIndx() < connectCell.getRowIndx() and cell.getColIndx() > connectCell.getColIndx()){
                    sideSet.insert(DOWNPOINT);
                    sideSet.insert(LEFTPOINT);
                }else{
                    cout << "has check exception " << endl;
                    cout << "cell : " << cell.getCellName() << " " << cell.getRowIndx() << " "<<   cell.getColIndx()<< endl;
                    cout << "connectCell : " << connectCell.getCellName() << " " <<connectCell.getRowIndx() << " " <<   connectCell.getColIndx()<< endl;
                }
            }
        };
        if(sideSet.size() > 3){
            isOnOneSide = false;
            break;
        }else{
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

