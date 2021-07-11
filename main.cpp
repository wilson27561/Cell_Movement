#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "flute/flute.h"
#include "Util/Constant.h"
#include "Util/Util.cpp"
#include "Util/ReadFile.cpp"
#include "Util/Evaluation.cpp"
#include "Util/ReRoute.cpp"
#include "Header/Layer.h"
#include "Header/MasterCell.h"
#include "Header/NumNonDefaultSupplyGgrid.h"
#include "Header/CellInstance.h"
#include "Header/Net.h"
#include "Header/SteinerPoint.h"

using namespace std;


int main() {

    Util util;
    ReadFile readFile;
    Evaluation evaluation;
    ReRoute reRoute;
    const string filePath = "case3.txt";
    string content;
    vector<string> contentvector;
    ifstream fin(FILEPATH);
//   case Data
    int maxCellMovent = 0;
    GgridBoundaryIndex ggridBoundaryIndex;
    map<string, Layer> layerMap;
    map<string, vector<int>> powerFactorMap;
    vector<NumNonDefaultSupplyGgrid> numNonDefaultSupplyVector;
    map<string, MasterCell> masterCellMap;
    map<string, CellInstance> cellInstanceMap;
    map<string, VoltageArea> voltageAreaMap;
    map<string, Net> netMap;
    map<string, int> boundaryMap;
    map<string, string> cellMap;
    map<string, Grid> gridMap;
    vector<vector<vector<int> > > gridVector;
    map<string, vector<string>> blockageCellMap;


    if (fin) {
        while (getline(fin, content)) {
            contentvector.push_back(content);
        }
    }
    fin.close();


    for (int i = 0; i < contentvector.size(); i++) {
        vector<string> lineVector = util.splitString(contentvector[i]);
        if (lineVector[0] == MAXCELLMOVE) {
            maxCellMovent = readFile.readMaxCell(lineVector);
        } else if (lineVector[0] == GGRIDBOUNDARYIDX) {
            ggridBoundaryIndex = readFile.readGGridBoundaryIdx(lineVector);
        } else if (lineVector[0] == NUMLAYER) {
            layerMap = readFile.readLayer(contentvector, i, lineVector[1]);
        } else if (lineVector[0] == NUMNONDEFAULTSUPPLYGGRID) {
            numNonDefaultSupplyVector = readFile.readNumNonDefaultSupply(contentvector, numNonDefaultSupplyVector, i,
                                                                         lineVector[1]);
        } else if (lineVector[0] == MASTERCELL) {
            masterCellMap = readFile.readMasterCell(contentvector, lineVector, masterCellMap, i);
            blockageCellMap = readFile.readBlockageCell(contentvector, lineVector, blockageCellMap, i);
        } else if (lineVector[0] == CELLINST) {
            cellInstanceMap = readFile.readCellInstance(lineVector, cellInstanceMap);
            blockageCellMap = readFile.readBlockageCell(lineVector, blockageCellMap);
            boundaryMap = readFile.readBoundary(lineVector, boundaryMap);
        } else if (lineVector[0] == NET) {
            netMap = readFile.readNet(contentvector, lineVector, netMap, masterCellMap, cellInstanceMap, i);
        } else if (lineVector[0] == NUMROUTES) {
            netMap = readFile.readRoute(contentvector, lineVector, netMap, i);
        } else if (lineVector[0] == NUMVOLTAGEAREA) {
            voltageAreaMap = readFile.readVoltageArea(contentvector, lineVector, voltageAreaMap, i);
        };
    }
//    int beforelengthScore = evaluation.wireLength(netMap);
    powerFactorMap = readFile.getLayerFacotr(layerMap, powerFactorMap);
    gridVector = readFile.getLayerGrid(ggridBoundaryIndex, layerMap, gridVector, numNonDefaultSupplyVector,blockageCellMap,cellInstanceMap,masterCellMap);
    gridVector = readFile.reduceRouteGridVector(gridVector, netMap);
    netMap = reRoute.boundaryReroute(netMap, boundaryMap, layerMap, cellInstanceMap, masterCellMap, gridVector,powerFactorMap);
    //    int rowGridEnd = ggridBoundaryIndex.getRowEndIdx();
//    int colGridEnd = ggridBoundaryIndex.getColEndIdx();
//    int layerSize = layerMap.size();
//    for (int layer = 0; layer < layerSize; layer++) {
//        for (int row = rowGridEnd - 1; row >= 0; row--) {
//            for (int col = 0; col < colGridEnd; col++) {
//                std::cout << gridVector[layer][row][col] << "\t";
//            }
//            std::cout << "" << std::endl;
//        }
//        std::cout << "" << std::endl;
//    }



//    int score = evaluation.evaluationScore(netMap,  layerMap);
//    int lengthScore = evaluation.wireLength(netMap);


//    cout << "before : " << beforelengthScore << " " << "after : "<< lengthScore << endl;

    //   program total wire length -> add weight -> powerFactor
//         int wire =   evaluation.wireLength(netMap);
//         cout << "wire length : " << wire << endl;
//         double score = evaluation.evaluationScore(netMap,layerMap);
//         cout << "score : " << score << endl;
//       Step 1 將所有 length 限制在boundary box內部
//    for (auto const &item : boundaryMap) {
//       cout << "key : "<<item.first << endl;
//       cout << "value : "<<item.second << endl;
//    }
//    for (auto const &item : cellMap) {
//       cout << "key : "<<item.first << endl;
//       cout << "value : "<<item.second << endl;
//    }
//    cout << "size : "<<netMap.size()<< endl;


}



//    int d=0;
//    int x[MAXD], y[MAXD];
//    Tree flutetree;
//    int flutewl;
//    while (!feof(stdin)) {
//        scanf("%d %d\n", &x[d], &y[d]);
//        std::cout<< "pin" << x[d]  << y[d] <<std::endl;
//        d++;
//    }
//    x[0] = 4;
//    y[0] = 1;
//    x[1] = 4;
//    y[1] = 4;
//    x[2] = 2;
//    y[2] = 2;
//    x[3] = 3;
//    y[3] = 3;
//    x[4] = 3;
//    y[4] = 1;
//    d=5;
//    readLUT();
//    flutetree = flute(d, x, y, ACCURACY);
//  std::cout<< "wirelength" << flutetree.length   <<std::endl;
//    printf("FLUTE wirelength = %d\n", flutetree.length);
//    plottree(flutetree);
//    flutewl = flute_wl(d, x, y, ACCURACY);
//    printf("FLUTE wirelength (without RSMT construction) = %d\n", flutewl);


//for(const auto& item :  netMap){
//vector<Route> routVec = item.second.getNumRoute();
//for (int i =0;i<item.second.getNumRoute().size(); i++) {
//cout << routVec[i].getStartRowIndx() <<" " << routVec[i].getStartColIndx() <<" "<<  routVec[i].getStartLayIndx() <<" "<< routVec[i].getEndRowIndx()<<" " << routVec[i].getEndColIndx()<<" " << routVec[i].getEndlayIndx() <<" "<< routVec[i].getNetName() << endl;
//}
//}




//make layer grid (目前先用vecotr去實作，三維陣列傳入副涵式方法不確定)
//    int rowGridBegin = ggridBoundaryIndex.getRowBeginIdx();
//    int colGridBegin = ggridBoundaryIndex.getColBeginIdx();
//    int rowGridEnd = ggridBoundaryIndex.getRowEndIdx();
//    int colGridEnd = ggridBoundaryIndex.getColEndIdx();
//    int layerSize = layerMap.size();
//    int ROWS = colGridEnd + 1;
//    int COLUMNS = colGridEnd + 1;
//    int LAYER = layerSize + 1;
//    int maze [LAYER][ROWS][COLUMNS];
//
//
//    for (int layer = 0; layer < LAYER; layer++) {
//        for (int row = rowGridBegin; row < ROWS; row++) {
//            for (int col = colGridBegin; col < COLUMNS; col++) {
//                maze[layer][row][col] = layerMap[to_string(layer)].getDefaultSupplyOfOneGrid();
//            }
//        }
//    }
//    for (auto const &numNonDefaultSupply : numNonDefaultSupplyVector) {
////        cout << item.getLayIndx() << " " << item.getRowIndx() << " " << item.getCollndx() << " " << item.getIncrOrDecrValue() << endl;
//        maze[numNonDefaultSupply.getLayIndx()][numNonDefaultSupply.getRowIndx()][numNonDefaultSupply.getCollndx()] = maze[numNonDefaultSupply.getLayIndx()][numNonDefaultSupply.getRowIndx()][numNonDefaultSupply.getCollndx()] +  numNonDefaultSupply.getIncrOrDecrValue();
//    }
//
//    for (int layer = 1; layer < LAYER; layer++) {
//        cout << "layer : " << layer << endl;
//        for (int row = 5; row >= 1; row--) {
//            for (int col = 1; col < COLUMNS; col++) {
//                std::cout << maze[layer][row][col] << "\t";
//            }
//            std::cout << "" << std::endl;
//        }
//        std::cout << "" << std::endl;
//    }