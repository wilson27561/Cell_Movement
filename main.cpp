#include <iostream>
#include <vector>
#include <map>
#include "flute/flute.h"
#include "Util/Constant.h"
#include "Util/Util.cpp"
#include "Util/ReadFile.cpp"
#include "Util/Evaluation.cpp"
#include "Util/ReRoute.cpp"
#include "Header/Layer.h"
#include "Header/MasterCell.h"
#include "Header/CellInstance.h"
#include "Header/Net.h"
#include "Header/SteinerPoint.h"

using namespace std;


int main() {

    double START,END;
    START = clock();

    Util util;
    ReadFile readFile;
    Evaluation evaluation;
    ReRoute reRoute;
    string content;
    vector<string> contentvector;
    int maxCellMovent = 0;
    GgridBoundaryIndex ggridBoundaryIndex;
    map<string, Layer> layerMap;
    map<string, vector<int>> powerFactorMap;
    map<string, MasterCell> masterCellMap;
    map<string, CellInstance> cellInstanceMap;
    map<string, VoltageArea> voltageAreaMap;
    map<string, Net> netMap;
    map<string, int> boundaryMap;
    map<string, Grid> gridMap;
    vector<vector<vector<int> > > gridVector;
    map<string,CellInstance> numMoveCellInstMap;
    map<string, map <string, Blockage>> blockageCellMap;

    ifstream fin(FILEPATH);
    if (fin) {
        while (getline(fin, content)) {
            contentvector.push_back(content);
        }
    }
    fin.close();

    //TODO 調整讀檔方式  --> 已調整
    //TODO 調整boundary map 改為每條線經過的 --> 已調整
    //TODO blockage 這部分要改一下 -> 已調整
    //TODO net supply 寫在 讀檔內 -> 已調整
    //TODO bug case1 cell 沒有連接在對的層 via部分改過
    //TODO output file
    //TODO bug case3
    //TODO random route
    //TODO 多執行緒 讀檔
    //TODO two pin 做 cell move
    //TODO 是否要將via 放到兩條線中間 ->不用
    //TODO 先檢查完需要做的reroute，再依net的weight順序做排序
    //TODO 確認minimumRoutingConstraint  ok (確認是否要從 1開始繞，還是可以從最minimumconstraint那一層開始去做繞線)


    for (int i = 0; i < contentvector.size(); i++) {
        vector<string> lineVector = util.splitString(contentvector[i]);
        if (lineVector[0] == MAXCELLMOVE) {
            cout << "max cell Movement start " << endl;
            maxCellMovent = readFile.readMaxCell(lineVector);
            cout << "max cell Movement end " << endl;
        } else if (lineVector[0] == GGRIDBOUNDARYIDX) {
            cout << "GridBoundary start " << endl;
            ggridBoundaryIndex = readFile.readGGridBoundaryIdx(lineVector);
            cout << "GridBoundary end " << endl;
        } else if (lineVector[0] == NUMLAYER) {
            cout << "-----layerMap start-----" << endl;
            readFile.readLayer(contentvector, &i, lineVector[1],&layerMap);
            cout << "-----layerMap end-----" << endl;
            gridVector = readFile.getLayerGrid(ggridBoundaryIndex, layerMap, gridVector);
        } else if (lineVector[0] == NUMNONDEFAULTSUPPLYGGRID) {
            cout << "-----NumNonDefaultSupply start-----" << endl;
            readFile.readNumNonDefaultSupply(contentvector,&gridVector,&i,lineVector[1]);
            cout << "-----NumNonDefaultSupply end-----" << endl;
        } else if (lineVector[0] == MASTERCELL) {
            cout << "-----MasterCell start-----" << endl;
            readFile.readMasterCell(contentvector, lineVector, &masterCellMap, &i);
            cout << "-----MasterCell end-----" << endl;
        } else if (lineVector[0] == CELLINST) {
            cout << "-----CellInstance start-----" << endl;
            readFile.readCellInstance(lineVector, &cellInstanceMap,&masterCellMap,&blockageCellMap,&gridVector);
            cout << "-----CellInstance end-----" << endl;
        } else if (lineVector[0] == NET) {
            cout << "-----net start-----" << endl;
            readFile.readNet(contentvector, lineVector, &netMap, masterCellMap, cellInstanceMap, &i);
            cout << "-----net end-----" << endl;
        } else if (lineVector[0] == NUMROUTES) {
            cout << "-----route start-----" << endl;
            readFile.readRoute(contentvector, lineVector, &netMap,&gridVector,&i);
            cout << "-----route end-----" << endl;
        } else if (lineVector[0] == NAME) {
            cout << "-----voltage start-----" << endl;
            readFile.readVoltageArea( contentvector,  &voltageAreaMap, &i);
            cout << "-----voltage end-----" << endl;
        };
    }
    powerFactorMap = readFile.getLayerFacotr(layerMap, powerFactorMap);
//    gridVector = readFile.reduceRouteGridVector(gridVector, netMap);
    netMap = reRoute.boundaryReroute(netMap, layerMap, cellInstanceMap, masterCellMap, gridVector,powerFactorMap);



//    ofstream myfile;
//    myfile.open ("output.txt");
//    myfile << "Writing this to a file.\n";
//    myfile.close();




    END = clock();
    cout << endl << "程式執行所花費：" << (double)clock()/CLOCKS_PER_SEC << " S";
    cout << endl << "進行運算所花費的時間：" << (END - START) / CLOCKS_PER_SEC << " S" << endl;

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