//
// Created by Ｗilson on 2021/8/9.
//

#include "CellMoveRoute.h"
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
            for (auto const connectNet : (*cellInstanceMap)[cell].getConnectNetVector()) {



                vector<RoutePoint> routePointVector;
                //取得每一點連線
                getRoutePointVector( connectNet, &(*netMap), &(*cellInstanceMap),&routePointVector);
                //如何移動cell 移動完失敗後 要再移回去



                // ----- reRoute Net start ------
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
                // ----- reRoute Net end ------
            }
        }
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



