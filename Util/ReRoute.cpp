//
// Created by Ｗilson on 2021/5/30.
//

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
#include "../Header/Net.h"
#include "../flute/flute.h"
#include "../Header/SteinerPoint.h"
#include "Constant.h"

using namespace std;

class GgridBoundaryIndex;

class Util;

class flute;

class ReRoute {

public:
    map<string, Net>
    boundaryReroute(map<string, Net> netMap, map<string, int> boundaryMap, map<string, Layer> layerMap,
                    map<string, CellInstance> cellInstanceMap, map<string, MasterCell> masterCellMap,
                    vector<vector<vector<int> > > gridVector,map<string, vector<int>> powerFactorMap) {
        //TODO 要對Map做排序嗎？ 這樣檢查每一條Net時可以從比重大的開始去做
        //TODO 先檢查完需要做的reroute，再依net的weight順序做排序
        //Step 1 ： 判斷每一條是否有超過boundary
        //TODO 確認要用edgeShift 還是 reroute 這裡先用reroute
        //Step 2 : Reroute 利用 兩個 cell 去做到reroute or edge shift
        //Step 3 : 利用FLUTE 去找到最小生成樹 的Steiner point
        //Step 4 : 生成新的VectorRoute 並且移除原本的線
        set<string> reRouteNet;
//        cout << "route net :" << netMap.size() << endl;
        for (auto const &item : netMap) {
            vector<Route> routeVec = item.second.getNumRoute();
            bool isNeedReroute = false;
            for (Route route : routeVec) {
                if (needReRoute(route, layerMap, boundaryMap)) {
//                    reRouteNet.insert(item.first);
                    isNeedReroute = true;
                    break;
                }
            }
            if(isNeedReroute){
                    vector<Route> routeVector = getSteinerRoute(item.first, netMap,gridVector, powerFactorMap,layerMap);
                    netMap[item.first].setNumRoute(routeVector);
            }
        }
        //Point 找到不好的繞線
        //Step 1 : 拿掉沒有用的繞線 example : N1 (2,2,2) => (2,2,1)
        //Step 2 : 找到out of boundary的點
        //Step 3 : rip-up and reroute -> edge shifting -> pattern route note 注意minimum routing
        //Step 4 : 判斷是否比原本的繞線還要好
//        for (string item :  reRouteNet) {
//            cout << "reRoute net :" << item << endl;
//            int row[MAXD];
//            int col[MAXD];
//            int index = 0;
//            //每一條net 上面connect 的 cell
//            for (auto const &cell : netMap[item].getConnectCell()) {
//                cout << cell.getCellName() << " " << cell.getConnectPin() << " " << cell.getRowIndx() << " "
//                     << cell.getColIndx() << " " << cell.getLayerName() << endl;
//                row[index] = cell.getRowIndx();
//                col[index] = cell.getColIndx();
//                index += 1;
//            }
//            readLUT();
//            Tree flutetree = flute(index, row, col, ACCURACY);
////            plottree(flutetree);
//            string minimumRoutingConstraint = netMap[item].getMinRoutingConstraint();
//            //TODO 還是要確認Cell是否在steiner point 上面
//            vector<SteinerPoint> steinerLine = getSteinerPointRoute(flutetree, gridVector, layerMap,powerFactorMap,minimumRoutingConstraint);
//
//            //TODO 最後繞線 ans routeVector
//            vector<Route> routeVector;
//            set<string> viaSet;
//            for (int index = 0; index < steinerLine.size(); index++) {
//                SteinerPoint steinerPoint = steinerLine[index];
//                gridVector = reduceSupply(gridVector, steinerPoint);
//                //處理minRoutingLayConstraint
//                if(index == 0 and minimumRoutingConstraint != "NoCstr"){
//                    cout << "minimumRoutingContraint : " << endl;
//                    routeVector =  getMinRoutingConstraint(routeVector, steinerPoint, minimumRoutingConstraint, item);
//                }
//                Route route;
//                route.setStartRowIndx(steinerPoint.getCellPointRow());
//                route.setStartColIndx(steinerPoint.getCellPointCol());
//                route.setEndRowIndx(steinerPoint.getSteinerPointRow());
//                route.setEndColIndx(steinerPoint.getSteinerPointCol());
//                route.setStartLayIndx(steinerPoint.getLayer());
//                route.setEndlayIndx(steinerPoint.getLayer());
//                route.setNetName(item);
//                routeVector.push_back(route);
//                if (index != 0) {
//                    int lastLayer = steinerLine[(index - 1)].getLayer();
//                    int layer = steinerLine[index].getLayer();
//                    if (layer != lastLayer) {
//                        Route route = getVia(steinerLine[(index - 1)], steinerLine[index], item,viaSet);
//                        string viaName = getViaName(route);
//                        //TODO 是否要將via 放到兩條線中間
//                        if(viaSet.count(viaName) == false){
//                            routeVector.push_back(route);
//                            viaSet.insert(viaName);
//                        }
//                    }
//                }
//            }
//            cout << "End route line : " << endl;
//            for(Route route: routeVector){
//                cout << "route line : " << route.getStartRowIndx() << " "
//                     << route.getStartColIndx() << " " << route.getStartLayIndx()
//                     << " " << route.getEndRowIndx() << " " << route.getEndColIndx() << " " << route.getEndlayIndx()
//                     << endl;
//            }
//            netMap[item].setNumRoute(routeVector);
//        }

        return netMap;
    }


    vector<Route> getSteinerRoute(string reRouteNet,map<string, Net> netMap,vector<vector<vector<int> > > gridVector,map<string, vector<int>> powerFactorMap,map<string, Layer> layerMap){
            vector<Route> routeVector;
            cout << "reRoute net :" << reRouteNet << endl;
            int row[MAXD];
            int col[MAXD];
            int index = 0;
            //每一條net 上面connect 的 cell
            for (auto const &cell : netMap[reRouteNet].getConnectCell()) {
//                cout << cell.getCellName() << " " << cell.getConnectPin() << " " << cell.getRowIndx() << " "
//                     << cell.getColIndx() << " " << cell.getLayerName() << endl;
                row[index] = cell.getRowIndx();
                col[index] = cell.getColIndx();
                index += 1;
            }
            readLUT();
            Tree flutetree = flute(index, row, col, ACCURACY);
//            plottree(flutetree);
            string minimumRoutingConstraint = netMap[reRouteNet].getMinRoutingConstraint();
            //TODO 還是要確認Cell是否在steiner point 上面
            vector<SteinerPoint> steinerLine = getSteinerPointRoute(flutetree, gridVector, layerMap,powerFactorMap,minimumRoutingConstraint);
            //TODO 最後繞線 ans routeVector
            set<string> viaSet;
            for (int index = 0; index < steinerLine.size(); index++) {
                SteinerPoint steinerPoint = steinerLine[index];
                gridVector = reduceSupply(gridVector, steinerPoint);
                //處理minRoutingLayConstraint
                if(index == 0 and minimumRoutingConstraint != "NoCstr"){
                    cout << "put in minimumRoutingContraint : " << endl;
                    routeVector =  getMinRoutingConstraint(routeVector, steinerPoint, minimumRoutingConstraint, reRouteNet);
                }
                Route route;
                route.setStartRowIndx(steinerPoint.getCellPointRow());
                route.setStartColIndx(steinerPoint.getCellPointCol());
                route.setEndRowIndx(steinerPoint.getSteinerPointRow());
                route.setEndColIndx(steinerPoint.getSteinerPointCol());
                route.setStartLayIndx(steinerPoint.getLayer());
                route.setEndlayIndx(steinerPoint.getLayer());
                route.setNetName(reRouteNet);
                routeVector.push_back(route);
                if (index != 0) {
                    int lastLayer = steinerLine[(index - 1)].getLayer();
                    int layer = steinerLine[index].getLayer();
                    if (layer != lastLayer) {
                        Route route = getVia(steinerLine[(index - 1)], steinerLine[index], reRouteNet,viaSet);
                        string viaName = getViaName(route);
                        //TODO 是否要將via 放到兩條線中間
                        if(viaSet.count(viaName) == false){
                            routeVector.push_back(route);
                            viaSet.insert(viaName);
                        }
                    }
                }
            }
            cout << "End route line : " << endl;
            for(Route route: routeVector){
                cout << "route line : " << route.getStartRowIndx() << " "
                     << route.getStartColIndx() << " " << route.getStartLayIndx()
                     << " " << route.getEndRowIndx() << " " << route.getEndColIndx() << " " << route.getEndlayIndx()
                     << endl;
            }
        return routeVector;
    }


    vector<Route> getMinRoutingConstraint(vector<Route> routeVector,SteinerPoint steinerPoint,string minimumRoutingConstraint,string netName){
        Route route;
        minimumRoutingConstraint.erase(std::remove(minimumRoutingConstraint.begin(), minimumRoutingConstraint.end(), 'M'), minimumRoutingConstraint.end());
        route.setStartRowIndx(steinerPoint.getCellPointRow());
        route.setStartColIndx(steinerPoint.getCellPointCol());
        route.setEndRowIndx(steinerPoint.getCellPointRow());
        route.setEndColIndx(steinerPoint.getCellPointCol());
        route.setStartLayIndx(1);
        route.setEndlayIndx(steinerPoint.getLayer());
        route.setNetName(netName);
        routeVector.push_back(route);

        return routeVector;
    }

    Route getVia(SteinerPoint lastSteinerPoint, SteinerPoint steinerPoint,string netName,set<string> viaSet) {
        Route route;
        int lastSteinerPointRow = lastSteinerPoint.getSteinerPointRow();
        int lastSteinerPointCol = lastSteinerPoint.getSteinerPointCol();
        int lastCellRow = lastSteinerPoint.getCellPointRow();
        int lastCellCol = lastSteinerPoint.getCellPointCol();
        int cellRow = steinerPoint.getCellPointRow();
        int cellCol = steinerPoint.getCellPointCol();
        int steinerPointRow = steinerPoint.getSteinerPointRow();
        int steinerPointCol = steinerPoint.getSteinerPointCol();
        if(cellRow == lastCellRow and cellCol == lastCellCol){
            route.setNetName(netName);
            route.setStartRowIndx(lastCellRow);
            route.setStartColIndx(lastCellCol);
            route.setEndRowIndx(cellRow);
            route.setEndColIndx(cellCol);
            route.setStartLayIndx(lastSteinerPoint.getLayer());
            route.setEndlayIndx(steinerPoint.getLayer());

        }else if(cellRow == lastSteinerPointRow and cellCol == lastSteinerPointCol){
            route.setNetName(netName);
            route.setStartRowIndx(lastSteinerPointRow);
            route.setStartColIndx(lastSteinerPointCol);
            route.setEndRowIndx(cellRow);
            route.setEndColIndx(cellCol);
            route.setStartLayIndx(lastSteinerPoint.getLayer());
            route.setEndlayIndx(steinerPoint.getLayer());

        }else if(steinerPointRow == lastCellRow and steinerPointCol == lastCellCol){
            route.setNetName(netName);
            route.setStartRowIndx(lastCellRow);
            route.setStartColIndx(lastCellCol);
            route.setEndRowIndx(steinerPointRow);
            route.setEndColIndx(steinerPointCol);
            route.setStartLayIndx(lastSteinerPoint.getLayer());
            route.setEndlayIndx(steinerPoint.getLayer());

        }else if(steinerPointRow == lastSteinerPointRow and steinerPointCol == lastSteinerPointCol){
            route.setNetName(netName);
            route.setStartRowIndx(lastSteinerPointRow);
            route.setStartColIndx(lastSteinerPointCol);
            route.setEndRowIndx(steinerPointRow);
            route.setEndColIndx(steinerPointCol);
            route.setStartLayIndx(lastSteinerPoint.getLayer());
            route.setEndlayIndx(steinerPoint.getLayer());

        }else if(steinerPointRow == lastCellRow and steinerPointCol == lastCellCol){
            route.setNetName(netName);
            route.setStartRowIndx(lastCellRow);
            route.setStartColIndx(lastCellCol);
            route.setEndRowIndx(steinerPointRow);
            route.setEndColIndx(steinerPointCol);
            route.setStartLayIndx(lastSteinerPoint.getLayer());
            route.setEndlayIndx(steinerPoint.getLayer());
        }else{
            cout << "via has some exception" << endl;
        }


        return route;
    }

    string getViaName(Route route){
          int startLayIndex =  route.getStartLayIndx();
          int endLayIndex = route.getEndlayIndx();
          string viaName;
          if(route.getStartLayIndx() > route.getEndlayIndx()){
              viaName =   to_string(route.getStartRowIndx()) +"_"+to_string(route.getStartColIndx())+"_"+ to_string(startLayIndex)+"_"+to_string(endLayIndex);
          }else{
               viaName =   to_string(route.getStartRowIndx()) +"_"+to_string(route.getStartColIndx())+"_"+ to_string(endLayIndex)+"_"+to_string(startLayIndex);
          }
          return viaName;
    }

    vector<vector<vector<int> > > reduceSupply(vector<vector<vector<int> > > gridVector, SteinerPoint steinerPoint) {
        int steinerPointRow = steinerPoint.getSteinerPointRow();
        int steinerPointCol = steinerPoint.getSteinerPointCol();
        int cellPointRow = steinerPoint.getCellPointRow();
        int cellPointCol = steinerPoint.getCellPointCol();
        int layer = steinerPoint.getLayer() - 1;
        int startCol = cellPointCol - 1;
        int endCol = steinerPointCol - 1;
        int startRow = cellPointRow - 1;
        int endRow = cellPointRow - 1;
        //Horizontal direction
        if (steinerPointRow == cellPointRow and steinerPointCol != cellPointCol) {
            if (steinerPointCol > cellPointCol) {
                for (int col = startCol; col <= endCol; col++) {
                    gridVector[layer][startRow][col] = gridVector[layer][startRow][col] - 1;
                };
            } else {
                for (int col = startCol; col >= endCol; col--) {
                    gridVector[layer][startRow][col] = gridVector[layer][startRow][col] - 1;
                }
            }
            //Vertical direction
        } else {
            if (steinerPointRow > cellPointRow) {
                for (int row = startRow; row <= endRow; row++) {
                    gridVector[layer][row][startCol] = gridVector[layer][row][startCol] - 1;
                }
            } else {
                for (int row = startRow; row >= endRow; row++) {
                    gridVector[layer][row][startCol] = gridVector[layer][row][startCol] - 1;
                }
            }
        }
        return gridVector;
    }


    //取得Steiner point cellpoint 為起點 steinerPoint 為終點
    //TODO 需要有give up route 的機制
    vector<SteinerPoint>
    getSteinerPointRoute(Tree t, vector<vector<vector<int> > > gridVector, map<string, Layer> layerMap,map <string,vector<int>> powerFactorMap,string minRoutingConstraint) {
        vector<int> layerPowerVectorH = powerFactorMap[HORIZONTAL];
        vector<int> layerPowerVectorV = powerFactorMap[VERTICAL];
        if(minRoutingConstraint != "NoCstr"){
            minRoutingConstraint.erase(std::remove(minRoutingConstraint.begin(), minRoutingConstraint.end(), 'M'), minRoutingConstraint.end());
           int constraint = stoi(minRoutingConstraint);
            for (int i = 0; i < layerPowerVectorH.size(); i++) {
                if(layerPowerVectorH[i]<constraint){
                    layerPowerVectorH.erase(layerPowerVectorH.begin() + i);
                }
            }
            for (int i = 0; i < layerPowerVectorV.size(); i++) {
                if(layerPowerVectorV[i]<constraint){
                    layerPowerVectorV.erase(layerPowerVectorV.begin() + i);
                }
            }

        }
        map<string, SteinerPoint> steinerMap;
        vector<SteinerPoint> steinerLineVector;
        bool giveUpRoute = true;
        int i;
        for (i = 0; i < 2 * t.deg - 2; i++) {
            string coordinate = to_string(t.branch[i].x) + "_" + to_string(t.branch[i].y);
            int steinerPointRow = t.branch[t.branch[i].n].x;
            int steinerPointCol = t.branch[t.branch[i].n].y;
            int cellPointRow = t.branch[i].x;
            int cellPointCol = t.branch[i].y;
            int routeLayer;
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
                            if (gridVector[(layerPowerVectorH[layer] - 1)][rowGrid][col] <= 0) {
                                canRoute = false;
                            }
                        }
                        if (canRoute) {
                            routeLayer = layerPowerVectorH[layer];
                            SteinerPoint steinerPoint(steinerPointRow, steinerPointCol, cellPointRow,
                                                      cellPointCol, routeLayer);
                            steinerMap.insert(pair<string, SteinerPoint>(coordinate, steinerPoint));
                            steinerLineVector.push_back(steinerPoint);
                            break;
                        }
                    }
//                    cout << "" << endl;
                } else {
                    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                        bool canRoute = true;
                        for (int col = startColGrid; col >= endColGrid; col--) {
//                            std::cout << gridVector[(layerPowerVectorH[layer] - 1)][rowGrid][col] << " ";
                            if (gridVector[(layerPowerVectorH[layer] - 1)][rowGrid][col] <= 0) {
                                canRoute = false;
                            }
                        }
                        if (canRoute) {
                            routeLayer = layerPowerVectorH[layer];
                            SteinerPoint steinerPoint(steinerPointRow, steinerPointCol, cellPointRow,
                                                      cellPointCol, routeLayer);
                            steinerMap.insert(pair<string, SteinerPoint>(coordinate, steinerPoint));
                            steinerLineVector.push_back(steinerPoint);
                            break;
                        }
                    }
//                    cout << "" << endl;
                }

            } else if (steinerPointCol == cellPointCol and steinerPointRow != cellPointRow) {
                int colGrid = cellPointCol - 1;
                int startRowGrid = cellPointRow - 1;
                int endRowGrid = steinerPointRow - 1;
                if (startRowGrid < endRowGrid) {
                    bool canRoute = true;
                    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                        for (int row = startRowGrid; row <= endRowGrid; row++) {
//                            std::cout << gridVector[(layerPowerVectorV[layer] - 1)][row][colGrid] << " ";
                            if (gridVector[(layerPowerVectorV[layer] - 1)][row][colGrid] <= 0) {
                                canRoute = false;
                            }
                        }
                        if (canRoute) {
                            routeLayer = layerPowerVectorV[layer];
                            SteinerPoint steinerPoint(steinerPointRow, steinerPointCol, cellPointRow,
                                                      cellPointCol, routeLayer);
                            steinerMap.insert(pair<string, SteinerPoint>(coordinate, steinerPoint));
                            steinerLineVector.push_back(steinerPoint);
                            break;
                        }
                    }
//                    cout << "" << endl;
                } else {
                    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                        bool canRoute = true;
                        for (int row = startRowGrid; row >= endRowGrid; row--) {
//                            std::cout << gridVector[(layerPowerVectorV[layer] - 1)][row][colGrid] << " ";
                            if (gridVector[(layerPowerVectorV[layer] - 1)][row][colGrid] <= 0) {
                                canRoute = false;
                            }
                        }
                        if (canRoute) {
                            routeLayer = layerPowerVectorV[layer];
                            SteinerPoint steinerPoint(steinerPointRow, steinerPointCol, cellPointRow,
                                                      cellPointCol, routeLayer);
                            steinerMap.insert(pair<string, SteinerPoint>(coordinate, steinerPoint));
                            steinerLineVector.push_back(steinerPoint);
                            break;
                        }
                    }
//                    cout << "" << endl;
                }
            } else if (steinerPointCol == cellPointCol and steinerPointRow == cellPointRow) {
                cout << "cellPointCol and steinerPointCol is on same Point" << endl;
            } else {
                //TODO 若Z pattern 走不了 之後要用U-pattern 走
                //TODO 若都不行 要give up
                cout << "get in side z pattern " << endl;
                int startRowGrid = cellPointRow - 1;
                int startColGrid = cellPointCol - 1;
                int endRowGrid = steinerPointRow - 1;
                int endColGrid = steinerPointCol - 1;
                cout << "cell : " << cellPointRow << " " << cellPointCol << " steinerPoint : " << steinerPointRow << " "
                     << steinerPointCol << endl;
                cout << " Cell to SteinerPoint" << endl;
                if (steinerPointRow > cellPointRow and steinerPointCol > cellPointCol) {
                    //左下到右上
                    steinerLineVector = bottomLeftToTopRight(steinerLineVector, startRowGrid, endRowGrid, startColGrid,
                                                             endColGrid, layerPowerVectorV, layerPowerVectorH,
                                                             gridVector);
                } else if (steinerPointRow > cellPointRow and steinerPointCol < cellPointCol) {
                    //右下到左上
                    steinerLineVector = bottomRightToTopLeft(steinerLineVector, startRowGrid, endRowGrid, startColGrid,
                                                             endColGrid, layerPowerVectorV, layerPowerVectorH,
                                                             gridVector);

                } else if (steinerPointRow < cellPointRow and steinerPointCol > cellPointCol) {
                    //左上到右下
                    steinerLineVector = topLeftToBottomRight(steinerLineVector, startRowGrid, endRowGrid, startColGrid,
                                                             endColGrid, layerPowerVectorV, layerPowerVectorH,
                                                             gridVector);

                } else {
                    //右上到左下
                    steinerLineVector = topRightToBottomLeft(steinerLineVector, startRowGrid, endRowGrid, startColGrid,
                                                             endColGrid, layerPowerVectorV, layerPowerVectorH,
                                                             gridVector);
                }
//                if (steinerLineVector.size() == 0) {
//                    giveUpRoute = false;
//                }
            }
//            if (giveUpRoute) {
//                cout << "inside give up route" << endl;
//                steinerMap.clear();
//                break;
//            }
        }
        return steinerLineVector;
    }


    vector<SteinerPoint>
    topRightToBottomLeft(vector<SteinerPoint> steinerLineVector, int startRowGrid, int endRowGrid, int startColGrid,
                         int endColGrid, vector<int> layerPowerVectorV, vector<int> layerPowerVectorH,
                         vector<vector<vector<int> > > gridVector) {
        SteinerPoint steinerPointFirst;
        SteinerPoint steinerPointSecond;
        SteinerPoint steinerPointThird;
        bool foundRoute = false;
        cout << "Top left to bottom right" << endl;

        //down-left-down
        for (int tempCol = startColGrid; tempCol >= endColGrid; tempCol--) {
            bool lineFirst = false;
            bool lineSecond = false;
            bool lineThird = false;

            if (lineFirst == false) {
                for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                    bool lackSupply = false;
                    for (int upCol = startColGrid; upCol >= tempCol; upCol--) {
                        cout << "up :" << gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][upCol] << endl;
                        if (gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][upCol] <= 0) {
                            lackSupply = false;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointFirst.setLayer(layerPowerVectorH[layer]);
                        steinerPointFirst.setCellPointRow(startRowGrid + 1);
                        steinerPointFirst.setCellPointCol(startColGrid + 1);
                        steinerPointFirst.setSteinerPointRow(startRowGrid + 1);
                        steinerPointFirst.setSteinerPointCol(tempCol + 1);
                        lineFirst = true;
                        cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
                             << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
                             << " "
                             << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer() << endl;
                        break;
                    }
                }
            }

            if (lineSecond == false) {
                for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                    bool lackSupply = false;
                    for (int row = startRowGrid; row >= endRowGrid; row--) {
                        cout << "V :" << gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] << endl;
                        if (gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointSecond.setLayer(layerPowerVectorV[layer]);
                        steinerPointSecond.setCellPointRow(startRowGrid + 1);
                        steinerPointSecond.setCellPointCol(tempCol + 1);
                        steinerPointSecond.setSteinerPointRow(endRowGrid + 1);
                        steinerPointSecond.setSteinerPointCol(tempCol + 1);
                        cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
                             << steinerPointSecond.getCellPointCol() << " " << steinerPointSecond.getSteinerPointRow()
                             << " "
                             << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer() << endl;
                        lineSecond = true;
                        break;
                    }
                }
            }
            if (lineThird == false) {
                for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                    bool lackSupply = false;
                    for (int downCol = tempCol; downCol >= endColGrid; downCol--) {
                        cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][downCol]
                             << endl;
                        if (gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][downCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointThird.setLayer(layerPowerVectorH[layer]);
                        steinerPointThird.setCellPointRow(endRowGrid + 1);
                        steinerPointThird.setCellPointCol(tempCol + 1);
                        steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                        steinerPointThird.setSteinerPointCol(endColGrid + 1);
                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
                             << " "
                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
                        lineThird = true;
                        break;
                    }

                }
            }
            if (lineFirst == true and lineSecond == true and lineThird == true) {
                if (countDistance(steinerPointFirst) > 1) {
                    steinerLineVector.push_back(steinerPointFirst);
                }
                if (countDistance(steinerPointSecond) > 1) {
                    steinerLineVector.push_back(steinerPointSecond);
                }
                if (countDistance(steinerPointThird) > 1) {
                    steinerLineVector.push_back(steinerPointThird);
                }
                foundRoute = true;
                cout << "Steiner line vector :" << endl;
                for (int i = 0; i < steinerLineVector.size(); i++) {
                    cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
                         << " " << steinerLineVector[i].getSteinerPointRow() << " "
                         << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer() << endl;
                }
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
                    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                        bool lackSupply = false;
                        for (int rightRow = startRowGrid; rightRow >= tempRow; rightRow--) {
                            cout << "left :" << gridVector[(layerPowerVectorV[layer] - 1)][rightRow][startColGrid]
                                 << endl;
                            if (gridVector[(layerPowerVectorV[layer] - 1)][rightRow][startColGrid] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointFirst.setLayer(layerPowerVectorV[layer]);
                            steinerPointFirst.setCellPointRow(startRowGrid + 1);
                            steinerPointFirst.setCellPointCol(startColGrid + 1);
                            steinerPointFirst.setSteinerPointRow(tempRow + 1);
                            steinerPointFirst.setSteinerPointCol(startColGrid + 1);
                            lineFirst = true;
                            cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
                                 << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
                                 << " "
                                 << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer()
                                 << endl;
                            break;
                        }
                    }
                }

                if (lineSecond == false) {
                    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                        bool lackSupply = false;
                        for (int col = startColGrid; col >= endColGrid; col--) {
                            cout << "H :" << gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] << endl;
                            if (gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] <= 0) {
                                lackSupply = false;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointSecond.setLayer(layerPowerVectorH[layer]);
                            steinerPointSecond.setCellPointRow(tempRow + 1);
                            steinerPointSecond.setCellPointCol(startColGrid + 1);
                            steinerPointSecond.setSteinerPointRow(tempRow + 1);
                            steinerPointSecond.setSteinerPointCol(endColGrid + 1);
                            cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
                                 << steinerPointSecond.getCellPointCol() << " "
                                 << steinerPointSecond.getSteinerPointRow()
                                 << " "
                                 << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer()
                                 << endl;
                            lineSecond = true;
                            break;
                        }
                    }
                }

                if (lineThird == false) {
                    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                        bool lackSupply = false;
                        for (int leftRow = tempRow; leftRow >= endRowGrid; leftRow--) {
                            cout << "right :" << gridVector[(layerPowerVectorV[layer] - 1)][leftRow][endColGrid]
                                 << endl;
                            if (gridVector[(layerPowerVectorV[layer] - 1)][leftRow][endColGrid] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointThird.setLayer(layerPowerVectorV[layer]);
                            steinerPointThird.setCellPointRow(tempRow + 1);
                            steinerPointThird.setCellPointCol(endColGrid + 1);
                            steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                            steinerPointThird.setSteinerPointCol(endColGrid + 1);
                            cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
                                 << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
                                 << " "
                                 << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer()
                                 << endl;
                            lineThird = true;
                            break;
                        }
                    }
                }
                cout << "" << endl;
                if (lineFirst == true and lineSecond == true and lineThird == true) {
                    if (countDistance(steinerPointFirst) > 1) {
                        steinerLineVector.push_back(steinerPointFirst);
                    }
                    if (countDistance(steinerPointSecond) > 1) {
                        steinerLineVector.push_back(steinerPointSecond);
                    }
                    if (countDistance(steinerPointThird) > 1) {
                        steinerLineVector.push_back(steinerPointThird);
                    }
                    foundRoute = true;
                    cout << "Steiner line vector :" << endl;
                    for (int i = 0; i < steinerLineVector.size(); i++) {
                        cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
                             << " " << steinerLineVector[i].getSteinerPointRow() << " "
                             << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
                             << endl;
                    }
                    break;
                }
            }
        }


        return steinerLineVector;
    }

    vector<SteinerPoint>
    topLeftToBottomRight(vector<SteinerPoint> steinerLineVector, int startRowGrid, int endRowGrid, int startColGrid,
                         int endColGrid, vector<int> layerPowerVectorV, vector<int> layerPowerVectorH,
                         vector<vector<vector<int>>> gridVector) {
        SteinerPoint steinerPointFirst;
        SteinerPoint steinerPointSecond;
        SteinerPoint steinerPointThird;
        bool foundRoute = false;

        cout << " Top right to bottom left" << endl;
        //Right-down-Right
        for (int tempCol = startColGrid; tempCol <= endColGrid; tempCol++) {
            bool lineFirst = false;
            bool lineSecond = false;
            bool lineThird = false;
            //
            if (lineFirst == false) {
                for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                    bool lackSupply = false;
                    for (int upCol = startColGrid; upCol <= tempCol; upCol++) {
                        cout << "up :" << gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][upCol] << endl;
                        if (gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][upCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointFirst.setLayer(layerPowerVectorH[layer]);
                        steinerPointFirst.setCellPointRow(startRowGrid + 1);
                        steinerPointFirst.setCellPointCol(startColGrid + 1);
                        steinerPointFirst.setSteinerPointRow(startRowGrid + 1);
                        steinerPointFirst.setSteinerPointCol(tempCol + 1);
                        lineFirst = true;
                        cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
                             << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
                             << " "
                             << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer() << endl;
                        break;
                    }
                }
            }

            if (lineSecond == false) {
                for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                    bool lackSupply = false;
                    for (int row = startRowGrid; row >= endRowGrid; row--) {
                        cout << "V :" << gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] << endl;
                        if (gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] <= 0) {
                            lackSupply = false;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointSecond.setLayer(layerPowerVectorV[layer]);
                        steinerPointSecond.setCellPointRow(startRowGrid + 1);
                        steinerPointSecond.setCellPointCol(tempCol + 1);
                        steinerPointSecond.setSteinerPointRow(endRowGrid + 1);
                        steinerPointSecond.setSteinerPointCol(tempCol + 1);
                        cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
                             << steinerPointSecond.getCellPointCol() << " " << steinerPointSecond.getSteinerPointRow()
                             << " "
                             << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer() << endl;
                        lineSecond = true;
                        break;
                    }
                }
            }

            if (lineThird == false) {
                for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                    bool lackSupply = false;
                    for (int downCol = tempCol; downCol <= endColGrid; downCol++) {
                        cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][downCol]
                             << endl;
                        if (gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][downCol] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointThird.setLayer(layerPowerVectorH[layer]);
                        steinerPointThird.setCellPointRow(endRowGrid + 1);
                        steinerPointThird.setCellPointCol(tempCol + 1);
                        steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                        steinerPointThird.setSteinerPointCol(endColGrid + 1);
                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
                             << " "
                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
                        lineThird = true;
                        break;
                    }
                }
            }
            cout << "" << endl;
            if (lineFirst == true and lineSecond == true and lineThird == true) {
                if (countDistance(steinerPointFirst) > 1) {
                    steinerLineVector.push_back(steinerPointFirst);
                }
                if (countDistance(steinerPointSecond) > 1) {
                    steinerLineVector.push_back(steinerPointSecond);
                }
                if (countDistance(steinerPointThird) > 1) {
                    steinerLineVector.push_back(steinerPointThird);
                }
                foundRoute = true;
                cout << "Steiner line vector :" << endl;
                for (int i = 0; i < steinerLineVector.size(); i++) {
                    cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
                         << " " << steinerLineVector[i].getSteinerPointRow() << " "
                         << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
                         << endl;
                }
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
                    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                        bool lackSupply = false;
                        for (int leftRow = startRowGrid; leftRow >= tempRow; leftRow--) {
                            cout << "left :" << gridVector[(layerPowerVectorV[layer] - 1)][leftRow][startColGrid]
                                 << endl;
                            if (gridVector[(layerPowerVectorV[layer] - 1)][leftRow][startColGrid] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointFirst.setLayer(layerPowerVectorV[layer]);
                            steinerPointFirst.setCellPointRow(startRowGrid + 1);
                            steinerPointFirst.setCellPointCol(startColGrid + 1);
                            steinerPointFirst.setSteinerPointRow(tempRow + 1);
                            steinerPointFirst.setSteinerPointCol(startColGrid + 1);
                            lineFirst = true;
                            cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
                                 << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
                                 << " "
                                 << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer()
                                 << endl;
                            break;
                        }
                    }
                }


                if (lineSecond == false) {
                    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                        bool lackSupply = false;
                        for (int col = startColGrid; col <= endColGrid; col++) {
                            cout << "H :" << gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] << endl;
                            if (gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointSecond.setLayer(layerPowerVectorH[layer]);
                            steinerPointSecond.setCellPointRow(startRowGrid + 1);
                            steinerPointSecond.setCellPointCol(startColGrid + 1);
                            steinerPointSecond.setSteinerPointRow(tempRow + 1);
                            steinerPointSecond.setSteinerPointCol(endColGrid + 1);
                            cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
                                 << steinerPointSecond.getCellPointCol() << " "
                                 << steinerPointSecond.getSteinerPointRow()
                                 << " "
                                 << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer()
                                 << endl;
                            lineSecond = true;
                            break;
                        }
                    }
                }

                if (lineThird == false) {
                    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                        bool lackSupply = false;
                        for (int rightRow = tempRow; rightRow >= endRowGrid; rightRow--) {
                            cout << "right :" << gridVector[(layerPowerVectorV[layer] - 1)][rightRow][endColGrid]
                                 << endl;
                            if (gridVector[(layerPowerVectorV[layer] - 1)][rightRow][endColGrid] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointThird.setLayer(layerPowerVectorV[layer]);
                            steinerPointThird.setCellPointRow(tempRow + 1);
                            steinerPointThird.setCellPointCol(endColGrid + 1);
                            steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                            steinerPointThird.setSteinerPointCol(endColGrid + 1);
                            cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
                                 << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
                                 << " "
                                 << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer()
                                 << endl;
                            lineThird = true;
                            break;
                        }
                    }
                    if (lineFirst == true and lineSecond == true and lineThird == true) {
                        if (countDistance(steinerPointFirst) > 1) {
                            steinerLineVector.push_back(steinerPointFirst);
                        }
                        if (countDistance(steinerPointSecond) > 1) {
                            steinerLineVector.push_back(steinerPointSecond);
                        }
                        if (countDistance(steinerPointThird) > 1) {
                            steinerLineVector.push_back(steinerPointThird);
                        }
                        foundRoute = true;
                        cout << "Steiner line vector :" << endl;
                        for (int i = 0; i < steinerLineVector.size(); i++) {
                            cout << steinerLineVector[i].getCellPointRow() << " "
                                 << steinerLineVector[i].getCellPointCol()
                                 << " " << steinerLineVector[i].getSteinerPointRow() << " "
                                 << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
                                 << endl;
                        }
                        break;
                    }
                }
                cout << "" << endl;
            }
        }

        return steinerLineVector;

    }


    vector<SteinerPoint>
    bottomRightToTopLeft(vector<SteinerPoint> steinerLineVector, int startRowGrid, int endRowGrid, int startColGrid,
                         int endColGrid, vector<int> layerPowerVectorV, vector<int> layerPowerVectorH,
                         vector<vector<vector<int>>

                         > gridVector) {
        SteinerPoint steinerPointFirst;
        SteinerPoint steinerPointSecond;
        SteinerPoint steinerPointThird;
        bool foundRoute = false;
        //右下到左上
        cout << " Bottom right to top left" << endl;
        //Up-Right-Up
        for (int tempRow = startRowGrid; tempRow <= endRowGrid; tempRow++) {
            bool lineFirst = false;
            bool lineSecond = false;
            bool lineThird = false;
            if (lineFirst == false) {
                for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                    bool lackSupply = false;
                    for (int rightRow = startRowGrid; rightRow <= tempRow; rightRow++) {
                        cout << "right :" << gridVector[(layerPowerVectorV[layer] - 1)][rightRow][startColGrid]
                             << endl;
                        if (gridVector[(layerPowerVectorV[layer] - 1)][rightRow][startColGrid] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointFirst.setLayer(layerPowerVectorV[layer]);
                        steinerPointFirst.setCellPointRow(startRowGrid + 1);
                        steinerPointFirst.setCellPointCol(startColGrid + 1);
                        steinerPointFirst.setSteinerPointRow(tempRow + 1);
                        steinerPointFirst.setSteinerPointCol(startColGrid + 1);
                        lineFirst = true;
                        cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
                             << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
                             << " "
                             << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer() << endl;
                        break;
                    }
                }
            }

            if (lineSecond == false) {
                for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                    bool lackSupply = false;
                    for (int col = startColGrid; col >= endColGrid; col--) {
                        cout << "H :" << gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] << endl;
                        if (gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointSecond.setLayer(layerPowerVectorH[layer]);
                        steinerPointSecond.setCellPointRow(tempRow + 1);
                        steinerPointSecond.setCellPointCol(startColGrid + 1);
                        steinerPointSecond.setSteinerPointRow(tempRow + 1);
                        steinerPointSecond.setSteinerPointCol(endColGrid + 1);
                        cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
                             << steinerPointSecond.getCellPointCol() << " " << steinerPointSecond.getSteinerPointRow()
                             << " "
                             << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer() << endl;
                        lineSecond = true;
                        break;
                    }
                }
            }

            if (lineThird == false) {
                for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                    bool lackSupply = false;
                    for (int leftRow = tempRow; leftRow <= endRowGrid; leftRow++) {
                        cout << "left :" << gridVector[(layerPowerVectorV[layer] - 1)][leftRow][endColGrid]
                             << endl;
                        if (gridVector[(layerPowerVectorV[layer] - 1)][leftRow][endColGrid] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointThird.setLayer(layerPowerVectorV[layer]);
                        steinerPointThird.setCellPointRow(tempRow + 1);
                        steinerPointThird.setCellPointCol(endColGrid + 1);
                        steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                        steinerPointThird.setSteinerPointCol(endColGrid + 1);
                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
                             << " "
                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
                        lineThird = true;
                        break;
                    }
                }
            }

            cout << "" << endl;
            if (lineFirst == true and lineSecond == true and lineThird == true) {
                if (countDistance(steinerPointFirst) > 1) {
                    steinerLineVector.push_back(steinerPointFirst);
                }
                if (countDistance(steinerPointSecond) > 1) {
                    steinerLineVector.push_back(steinerPointSecond);
                }
                if (countDistance(steinerPointThird) > 1) {
                    steinerLineVector.push_back(steinerPointThird);
                }
                foundRoute = true;
                cout << "Steiner line vector :" << endl;
                for (int i = 0; i < steinerLineVector.size(); i++) {
                    cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
                         << " " << steinerLineVector[i].getSteinerPointRow() << " "
                         << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer() << endl;
                }
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
                    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                        bool lackSupply = false;
                        for (int downCol = startColGrid; downCol >= tempCol; downCol--) {
                            cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][downCol]
                                 << endl;
                            if (gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][downCol] <= 0) {
                                lackSupply = false;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointFirst.setLayer(layerPowerVectorH[layer]);
                            steinerPointFirst.setCellPointRow(startRowGrid + 1);
                            steinerPointFirst.setCellPointCol(startColGrid + 1);
                            steinerPointFirst.setSteinerPointRow(startRowGrid + 1);
                            steinerPointFirst.setSteinerPointCol(tempCol + 1);
                            lineFirst = true;
                            cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
                                 << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
                                 << " "
                                 << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer()
                                 << endl;
                            break;
                        }
                    }
                }

                if (lineSecond == false) {
                    bool lackSupply = false;
                    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                        for (int row = startRowGrid; row <= endRowGrid; row++) {
                            cout << "V :" << gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] << endl;
                            if (gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointSecond.setLayer(layerPowerVectorV[layer]);
                            steinerPointSecond.setCellPointRow(startRowGrid + 1);
                            steinerPointSecond.setCellPointCol(tempCol + 1);
                            steinerPointSecond.setSteinerPointRow(endRowGrid + 1);
                            steinerPointSecond.setSteinerPointCol(tempCol + 1);
                            cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
                                 << steinerPointSecond.getCellPointCol() << " "
                                 << steinerPointSecond.getSteinerPointRow()
                                 << " "
                                 << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer()
                                 << endl;
                            lineSecond = true;
                            break;
                        }
                    }
                }


                if (lineThird == false) {
                    bool lackSupply = false;
                    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                        for (int upCol = tempCol; upCol >= endColGrid; upCol--) {
                            cout << "up :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][upCol] << endl;
                            if (gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][upCol] <= 0) {
                                lackSupply = false;
                            }
                        }

                        if (lackSupply == false) {
                            steinerPointThird.setLayer(layerPowerVectorH[layer]);
                            steinerPointThird.setCellPointRow(endRowGrid + 1);
                            steinerPointThird.setCellPointCol(tempCol + 1);
                            steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                            steinerPointThird.setSteinerPointCol(endColGrid + 1);
                            cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
                                 << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
                                 << " "
                                 << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer()
                                 << endl;
                            lineThird = true;
                            break;
                        }
                    }
                }
                cout << "" << endl;
                if (lineFirst == true and lineSecond == true and lineThird == true) {
                    if (countDistance(steinerPointFirst) > 1) {
                        steinerLineVector.push_back(steinerPointFirst);
                    }
                    if (countDistance(steinerPointSecond) > 1) {
                        steinerLineVector.push_back(steinerPointSecond);
                    }
                    if (countDistance(steinerPointThird) > 1) {
                        steinerLineVector.push_back(steinerPointThird);
                    }
                    foundRoute = true;
                    cout << "Steiner line vector :" << endl;
                    for (int i = 0; i < steinerLineVector.size(); i++) {
                        cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
                             << " " << steinerLineVector[i].getSteinerPointRow() << " "
                             << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
                             << endl;
                    }
                    break;
                }
            }
        }
        return steinerLineVector;
    }


    vector<SteinerPoint>
    bottomLeftToTopRight(vector<SteinerPoint> steinerLineVector, int startRowGrid, int endRowGrid, int startColGrid,
                         int endColGrid, vector<int> layerPowerVectorV, vector<int> layerPowerVectorH,
                         vector<vector<vector<int>>

                         > gridVector) {
        SteinerPoint steinerPointFirst;
        SteinerPoint steinerPointSecond;
        SteinerPoint steinerPointThird;
        bool foundRoute = false;
        cout << " Bottom left to top right" << endl;
        //Up-Right-Up
        //向上走 每換一次Row 代表 換不同的 pattern route 的 方法
        for (int tempRow = startRowGrid; tempRow <= endRowGrid; tempRow++) {
            bool lineFirst = false;
            bool lineSecond = false;
            bool lineThird = false;
            //當此pattern 可以找到不同層的線，即可完成pattern Route
            if (lineFirst == false) {
                for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                    bool lackSupply = false;
                    for (int leftRow = startRowGrid; leftRow <= tempRow; leftRow++) {
                        cout << "left :" << gridVector[(layerPowerVectorV[layer]) - 1][leftRow][startColGrid] << endl;
                        if (gridVector[(layerPowerVectorV[layer] - 1)][leftRow][startColGrid] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointFirst.setLayer(layerPowerVectorV[layer]);
                        steinerPointFirst.setCellPointRow(startRowGrid + 1);
                        steinerPointFirst.setCellPointCol(startColGrid + 1);
                        steinerPointFirst.setSteinerPointRow(tempRow + 1);
                        steinerPointFirst.setSteinerPointCol(startColGrid + 1);
                        lineFirst = true;
                        cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
                             << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
                             << " "
                             << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer() << endl;
                        break;
                    }
                }
            }
            //向右走
            if (lineSecond == false) {
                for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                    bool lackSupply = false;
                    for (int col = startColGrid; col <= endColGrid; col++) {
                        cout << "H :" << gridVector[(layerPowerVectorH[layer] - 1)][tempRow][col] << endl;
                        if (gridVector[layerPowerVectorH[layer]][tempRow][col] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointSecond.setLayer(layerPowerVectorH[layer]);
                        steinerPointSecond.setCellPointRow(tempRow + 1);
                        steinerPointSecond.setCellPointCol(startColGrid + 1);
                        steinerPointSecond.setSteinerPointRow(tempRow + 1);
                        steinerPointSecond.setSteinerPointCol(endColGrid + 1);
                        cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
                             << steinerPointSecond.getCellPointCol() << " " << steinerPointSecond.getSteinerPointRow()
                             << " "
                             << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer() << endl;
                        lineSecond = true;
                        break;
                    }
                }
            }
            //向上走
            if (lineThird == false) {
                for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                    bool lackSupply = false;
                    for (int rightRow = tempRow; rightRow <= endRowGrid; rightRow++) {
                        cout << "right :" << gridVector[(layerPowerVectorV[layer] - 1)][rightRow][endColGrid]
                             << endl;
                        if (gridVector[(layerPowerVectorV[layer] - 1)][rightRow][endColGrid] <= 0) {
                            lackSupply = true;
                        }
                    }
                    if (lackSupply == false) {
                        steinerPointThird.setLayer(layerPowerVectorV[layer]);
                        steinerPointThird.setCellPointRow(tempRow + 1);
                        steinerPointThird.setCellPointCol(endColGrid + 1);
                        steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                        steinerPointThird.setSteinerPointCol(endColGrid + 1);
                        cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
                             << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
                             << " "
                             << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer() << endl;
                        lineThird = true;
                        break;
                    }
                }
            }
            cout << "" << endl;
            if (lineFirst == true and lineSecond == true and lineThird == true) {
                if (countDistance(steinerPointFirst) > 1) {
                    steinerLineVector.push_back(steinerPointFirst);
                }
                if (countDistance(steinerPointSecond) > 1) {
                    steinerLineVector.push_back(steinerPointSecond);
                }
                if (countDistance(steinerPointThird) > 1) {
                    steinerLineVector.push_back(steinerPointThird);
                }
                foundRoute = true;
                cout << "Steiner line vector :" << endl;
                for (int i = 0; i < steinerLineVector.size(); i++) {
                    cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
                         << " " << steinerLineVector[i].getSteinerPointRow() << " "
                         << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer() << endl;
                }
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
                    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                        bool lackSupply = false;
                        for (int downCol = startColGrid; downCol <= tempCol; downCol++) {
                            cout << "down :" << gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][downCol]
                                 << endl;
                            if (gridVector[(layerPowerVectorH[layer] - 1)][startRowGrid][downCol] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointFirst.setLayer(layerPowerVectorH[layer]);
                            steinerPointFirst.setCellPointRow(startRowGrid + 1);
                            steinerPointFirst.setCellPointCol(startColGrid + 1);
                            steinerPointFirst.setSteinerPointRow(startRowGrid + 1);
                            steinerPointFirst.setSteinerPointCol(tempCol + 1);
                            lineFirst = true;
                            cout << "Steiner line : " << steinerPointFirst.getCellPointRow() << " "
                                 << steinerPointFirst.getCellPointCol() << " " << steinerPointFirst.getSteinerPointRow()
                                 << " "
                                 << steinerPointFirst.getSteinerPointCol() << " " << steinerPointFirst.getLayer()
                                 << endl;
                            break;
                        }
                    }
                }
                if (lineSecond == false) {
                    for (int layer = 0; layer < layerPowerVectorV.size(); layer++) {
                        bool lackSupply = false;
                        for (int row = startRowGrid; row <= endRowGrid; row++) {
                            cout << "V :" << gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] << endl;
                            if (gridVector[(layerPowerVectorV[layer] - 1)][row][tempCol] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointSecond.setLayer(layerPowerVectorV[layer]);
                            steinerPointSecond.setCellPointRow(startRowGrid + 1);
                            steinerPointSecond.setCellPointCol(tempCol + 1);
                            steinerPointSecond.setSteinerPointRow(endRowGrid + 1);
                            steinerPointSecond.setSteinerPointCol(tempCol + 1);
                            cout << "Steiner line : " << steinerPointSecond.getCellPointRow() << " "
                                 << steinerPointSecond.getCellPointCol() << " "
                                 << steinerPointSecond.getSteinerPointRow()
                                 << " "
                                 << steinerPointSecond.getSteinerPointCol() << " " << steinerPointSecond.getLayer()
                                 << endl;
                            lineSecond = true;
                            break;
                        }
                    }
                }
                if (lineThird == false) {
                    for (int layer = 0; layer < layerPowerVectorH.size(); layer++) {
                        bool lackSupply = false;
                        for (int upCol = tempCol; upCol <= endColGrid; upCol++) {
                            cout << "up :" << gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][upCol] << endl;
                            if (gridVector[(layerPowerVectorH[layer] - 1)][endRowGrid][upCol] <= 0) {
                                lackSupply = true;
                            }
                        }
                        if (lackSupply == false) {
                            steinerPointThird.setLayer(layerPowerVectorH[layer]);
                            steinerPointThird.setCellPointRow(endRowGrid + 1);
                            steinerPointThird.setCellPointCol(tempCol + 1);
                            steinerPointThird.setSteinerPointRow(endRowGrid + 1);
                            steinerPointThird.setSteinerPointCol(endColGrid + 1);
                            cout << "Steiner line : " << steinerPointThird.getCellPointRow() << " "
                                 << steinerPointThird.getCellPointCol() << " " << steinerPointThird.getSteinerPointRow()
                                 << " "
                                 << steinerPointThird.getSteinerPointCol() << " " << steinerPointThird.getLayer()
                                 << endl;
                            lineThird = true;
                            break;
                        }
                    }
                }
                cout << "" << endl;
                if (lineFirst == true and lineSecond == true and lineThird == true) {
                    if (countDistance(steinerPointFirst) > 1) {
                        steinerLineVector.push_back(steinerPointFirst);
                    }
                    if (countDistance(steinerPointSecond) > 1) {
                        steinerLineVector.push_back(steinerPointSecond);
                    }
                    if (countDistance(steinerPointThird) > 1) {
                        steinerLineVector.push_back(steinerPointThird);
                    }
                    foundRoute = true;
                    cout << "Steiner line vector :" << endl;
                    for (int i = 0; i < steinerLineVector.size(); i++) {
                        cout << steinerLineVector[i].getCellPointRow() << " " << steinerLineVector[i].getCellPointCol()
                             << " " << steinerLineVector[i].getSteinerPointRow() << " "
                             << steinerLineVector[i].getSteinerPointCol() << " " << steinerLineVector[i].getLayer()
                             << endl;
                    }
                    break;
                }

            }
        }
        return steinerLineVector;
    }


    int countDistance(SteinerPoint steinerPoint) {
        int totaly = steinerPoint.getCellPointCol() - steinerPoint.getSteinerPointCol();
        int totalx = steinerPoint.getCellPointRow() - steinerPoint.getSteinerPointRow();
        totaly = abs(totaly) + 1;
        totalx = abs(totalx);
        return (totaly + totalx);
    };

//    TODO 確認routingLayer
    int getRouteLayer(map<string, Layer> layerMap, CellInstance cell, string direction) {
        if (direction == HORIZONTAL) {
            return 3;
        } else {
            return 2;
        }
    }

    Route
    minRoutingConstraintRoute(int minRoutingConstraint, CellInstance cell, map<string, SteinerPoint> steinerMap,
                              map<string, Layer> layerMap, string reRouteNet, vector<Route> reRouteVector) {
        Route route;
        route.setStartRowIndx(cell.getRowIndx());
        route.setStartColIndx(cell.getColIndx());
        route.setStartLayIndx(1);
        route.setEndRowIndx(cell.getRowIndx());
        route.setEndColIndx(cell.getColIndx());
        int startLayer = startMinRoutingConstraint(cell, minRoutingConstraint, steinerMap, layerMap);
        route.setEndlayIndx(startLayer);
        route.setNetName(reRouteNet);
        return route;
    }

    //TODO 之後要處理 MinRoutingConstraint
    int startMinRoutingConstraint(CellInstance cell, int minRoutingConstraint, map<string, SteinerPoint> steinerMap,
                                  map<string, Layer> layerMap) {
        if (minRoutingConstraint > cell.getColIndx()) {
            return minRoutingConstraint;
        } else if (minRoutingConstraint < cell.getColIndx()) {
            return cell.getColIndx();
        } else {
            return cell.getColIndx();
        }
    }

    //判斷是否超過boundary
    bool needReRoute(Route route, map<string, Layer> layerMap, map<string, int> boundaryMap) {
        bool isReRoute = false;
        //TODO 暫時先不用判斷via 直接整條ReRoute
        if (route.getStartLayIndx() == route.getEndlayIndx()) {
            if (isOutOfBoundary(route, boundaryMap)) {
//                cout << route.getNetName() << endl;
//                cout << route.getStartRowIndx() << " " << route.getStartColIndx() << " " << route.getEndRowIndx() << " "
//                     << route.getEndColIndx() << endl;
                isReRoute = true;
            }
        }
        return isReRoute;
    }

    //判斷每個點是否超過boudary
    bool isOutOfBoundary(Route route, map<string, int> boundaryMap) {
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

    string transferIndexToName(int layIndex) {
        string M = "M";
        return M + to_string(layIndex);
    }

};
