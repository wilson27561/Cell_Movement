//
// Created by Ｗilson on 2021/5/15.
//

#include <iostream>
#include <vector>
#include <map>
#include "../Header/GgridBoundaryIndex.h"
#include "../Header/Layer.h"
#include "../Header/MasterCell.h"
#include "../Header/NumNonDefaultSupplyGgrid.h"
#include "../Header/CellInstance.h"
#include "../Header/VoltageArea.h"
#include "../Header/Net.h"
#include "Constant.h"
using namespace std;
class GgridBoundaryIndex;
class Util;

class ReadFile{
public :
    int readMaxCell(vector<string> lineVector){
        return  atoi(lineVector[1].c_str());
    }
    GgridBoundaryIndex readGGridBoundaryIdx(vector<string> lineVector){
        GgridBoundaryIndex gridBoundaryIndex;
        gridBoundaryIndex.setRowBeginIdx(atoi(lineVector[1].c_str()));
        gridBoundaryIndex.setColBeginIdx(atoi(lineVector[2].c_str()));
        gridBoundaryIndex.setRowEndIdx(atoi(lineVector[3].c_str()));
        gridBoundaryIndex.setRowEndIdx(atoi(lineVector[4].c_str()));
        return gridBoundaryIndex;
    }
    map<string,Layer> readLayer(vector<string> contentVector,int index,string layerCount){
        map<string,Layer> layerMap;
        int mapSize = index + atoi(layerCount.c_str());
        for (int i = index+1; i<=mapSize;i++){
            vector<string> lineVector =  splitString(contentVector[i]);
            Layer layer;
            layer.setLayerName(lineVector[1]);
            layer.setIndex(stoi(lineVector[2]));
            layer.setRoutingDirection(lineVector[3]);
            layer.setDefaultSupplyOfOneGrid(stoi(lineVector[4]));
            layer.setPowerFactor(stod(lineVector[4]));
//           Map M1,Layer
            layerMap.insert(pair<string,Layer>(lineVector[1],layer));
        };
        return layerMap;
    }
    map<string,vector<NumNonDefaultSupplyGgrid>> readNumNonDefaultSupply(vector<string> contentVector,map<string,vector<NumNonDefaultSupplyGgrid>> numNonDefaultSupplyMap,int index,string layerCount){
        int mapSize = index + stoi(layerCount);
        //  use vector to put NumNonDefaultSupplyGgrid
        for (int i = index+1;i<=mapSize;i++){
            vector<string> lineVector =  splitString(contentVector[i]);
            NumNonDefaultSupplyGgrid numNonDefaultSupply;
            string layerName = LAYERPREFIX_M+lineVector[2];
            numNonDefaultSupply.setRowIndx(stoi(lineVector[0]));
            numNonDefaultSupply.setCollndx(stoi(lineVector[1]));
            numNonDefaultSupply.setLayIndx(stoi(lineVector[2]));
            numNonDefaultSupply.setIncrOrDecrValue(stoi(lineVector[3]));
            numNonDefaultSupply.setLayerName(layerName);
            if(numNonDefaultSupplyMap.count(layerName)){
                numNonDefaultSupplyMap[layerName].push_back(numNonDefaultSupply);
            }else {
                vector<NumNonDefaultSupplyGgrid> numNonDefaultSupplygridVector;
                numNonDefaultSupplygridVector.push_back(numNonDefaultSupply);
                numNonDefaultSupplyMap.insert(
                        pair<string, vector<NumNonDefaultSupplyGgrid>>(layerName, numNonDefaultSupplygridVector));
            }
        }
        return numNonDefaultSupplyMap;
    }
    map<string,MasterCell> readMasterCell(vector<string> contentVector,vector<string> lineVector, map<string,MasterCell> masterCellMap,int index){
      string masterCellName = lineVector[1];
      int pinCount =stoi(lineVector[2]);
      int blockageCount = stoi(lineVector[3]);

      MasterCell masterCell;
      masterCell.setMasterCellName(masterCellName);
      //PinCount
      for(int i = index+1;i<=index+pinCount;i++){
          vector<string> pinVector =  splitString(contentVector[i]);
          map<string, Pin> pinMap;
          Pin pin;
          pin.setPinName(pinVector[1]);
          pin.setLayer(pinVector[2]);
          pinMap.insert(pair<string,Pin>(pinVector[1],pin));
          masterCell.setPinType(pinMap);
      }
      //BlockageCount
      for(int i = index+pinCount+1;i<=index+pinCount+blockageCount;i++){
          vector<string> blockageVector =   splitString(contentVector[i]);
          map<string,Blockage> blockageMap;
          Blockage blockage;
          blockage.setBlockageName(blockageVector[1]);
          blockage.setBlockageLayer(blockageVector[2]);
          blockage.setDemand(stoi(blockageVector[3]));
          blockageMap.insert(pair<string,Blockage>(blockageVector[1],blockage));
          masterCell.setBlockageType(blockageMap);
      }
        return masterCellMap;
    }


    map<string,CellInstance> readCellInstance(vector<string> lineVector, map<string,CellInstance> cellInstanceMap){
            CellInstance cellInstance;
            cellInstance.setCellName(lineVector[1]);
            //CellMasterName;
            cellInstance.setMasterCellName(lineVector[2]);
            cellInstance.setRowIndx(stoi(lineVector[3]));
            cellInstance.setColIndx(stoi(lineVector[4]));
            cellInstance.setMovalbe(lineVector[5]);
            cellInstanceMap.insert(pair<string,CellInstance>(lineVector[1],cellInstance));
        return cellInstanceMap;
    };
    map<string,Net> readNet(vector<string> contentVector,vector<string> lineVector , map<string,Net> netMap,int index){
        int pinCount = stoi(lineVector[2]);
        Net net;
        net.setNetName(lineVector[1]);
        net.setMinRoutingConstraint(lineVector[3]);
        net.setWeight(stod(lineVector[4]));
        map<string,string> cellPinMap;
        for (int i = index+1; i <= index+pinCount ; i++) {
            vector<string> pinVector = splitString(contentVector[i]);
            vector<string> pinCellVec = splitByChar(pinVector[1],'/');
            cellPinMap.insert(pair<string,string>(pinCellVec[0],pinCellVec[1]));
        }
        netMap.insert(pair<string,Net>(lineVector[1],net));

        return netMap;
    }
    map<string,Net> readRoute(vector<string> contentVector,vector<string> lineVector , map<string,Net> netMap,int index) {
        int numRoutes = stoi(lineVector[1]);
        for (int i = index+1; i <= index+numRoutes; i++) {
           vector<string> routeVector = splitString(contentVector[i]);
            if(netMap.count(routeVector[6])){
               Route route;
               route.setNetName(routeVector[6]);
               route.setStartRowIndx(stoi(routeVector[0]));
               route.setStartColIndx(stoi(routeVector[1]));
               route.setStartLayIndx(stoi(routeVector[2]));
               route.setEndRowIndx(stoi(routeVector[3]));
               route.setEndColIndx(stoi(routeVector[4]));
               route.setEndlayIndx(stoi(routeVector[5]));
               vector<Route> routingVector = netMap[routeVector[6]].getNumRoute();
               routingVector.push_back(route);
//                cout <<"vec size : " << routeVector[6]<< endl;
               netMap[routeVector[6]].setNumRoute(routingVector);
//               cout <<"size : " << netMap[routeVector[6]].getNumRoute().size()<< endl;
//               netMap[routeVector[6]].getNumRoute().push_back(route);
            }
        }
        return netMap;
    }

    map<string,VoltageArea> readVoltageArea(vector<string> contentVector,vector<string> lineVector, map<string,VoltageArea>  voltageAreaMap,int index){
        VoltageArea voltageArea;
        vector<Grid> gridVector;
        vector<string> instanceVector;
        for (int i = index+1; i < contentVector.size() ; i++) {
            vector<string> voltage = splitString(contentVector[i]);
            if(voltage[0] == NAME){
                voltageArea.setAreaName(voltage[1]);
            }else if(voltage[0] == GGRIDS){
                int gridCount = stoi(voltage[1]);
                for (int j = i+1; j <=i+gridCount ; j++) {
                    vector<string> voltage = splitString(contentVector[j]);
                    Grid grid;
                    grid.setRowIndx(stoi(voltage[0]));
                    grid.setColIndx(stoi(voltage[1]));
                    gridVector.push_back(grid);
                }
                voltageArea.setGridVector(gridVector);
                i = i+gridCount;
            }else if(voltage[0] == INSTANCES){
                int instanceCount = stoi(voltage[1]);
                for (int j = i+1;j<=i+instanceCount;j++){
                    vector<string> voltage = splitString(contentVector[j]);
                    instanceVector.push_back(voltage[0]);
                }
                voltageArea.setInstance(instanceVector);
                i = i + instanceCount;
            }
        }
        return voltageAreaMap;
    }


    vector<string> splitByChar(string str,char pattern){
        stringstream test(str);
        string segment;
        vector<string> seglist;
        while(std::getline(test, segment, pattern))
        {
            seglist.push_back(segment);
        }
        return seglist;
    }

    //之後要改掉(此為Util 無法引入)
    vector<string> splitString( string content)
    {
        int i = 0;
        vector<string> contentVector;
        stringstream contentArray(content);
        string word;
        while (contentArray.good())
        {
            contentArray >> word ;
            contentVector.push_back(word);
            ++i;
        }
        return contentVector;
    }


};

