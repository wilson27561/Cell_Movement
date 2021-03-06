#include <iostream>
#include <vector>
#include <map>
#include <fstream>
//#include "flute/flute.h"
#include "Util/Constant.h"
#include "Util/ReadFile.h"
#include "Util/ReRoute.h"
#include "Header/Layer.h"
#include "Header/MasterCell.h"
#include "Header/CellInstance.h"
#include "Header/Net.h"
#include "Header/SteinerPoint.h"
#include "Util/CellMoveRoute.h"

using namespace std;

void printGridVector(vector<vector<vector<int> > > *gridVector) {
//     ----- supply cout start -----
    for (int layer = 1; layer <= (*gridVector).size(); layer++) {
        cout << "Layer Name : " << layer << endl;
        for (int row = (*gridVector)[1].size(); row >= 1; row--) {
            for (int col = 1; col <= (*gridVector)[1][1].size(); col++) {
                cout << (*gridVector)[layer - 1][row - 1][col - 1] << "\t";
            }
            cout << endl;
        }
        cout << endl;
    }
//     ----- supply cout end -----
}

vector<string> splitString(string content) //MasterCell MC1 2 2
{
    int i = 0;
    vector<string> contentVector;
    stringstream contentArray(content);
    string word;
    while (contentArray.good()) {
        contentArray >> word;
        contentVector.push_back(word);
        ++i;
    }
    return contentVector;
}


int main(int argc, char *argv[]) {


    double START, END;
    START = clock();

//    string inputData = argv[1];
//    string outputData = argv[2];


    ReadFile readFile;
    ReRoute reRoute;
    CellMoveRoute cellMoveRoute;
    string content;
    vector<string> contentvector;
    int maxCellMovent = 0;
    GgridBoundaryIndex ggridBoundaryIndex;
    map<string, Layer> layerMap;
    map<string, vector<int> > powerFactorMap;
    map<string, MasterCell> masterCellMap;
    map<string, CellInstance> cellInstanceMap;
    unordered_map <string, VoltageArea> voltageAreaMap;
    map<string, Net> netMap;
    vector<vector<vector<int> > > gridVector;
    map<string, CellInstance> numMoveCellInstMap;
    map<string, map<string, Blockage> > blockageCellMap;
    set<string> netNameSet;
    vector<string> emptyBlockageCellVector;
    //reduce Supply
    map<string, set<string> > reducePointMap;
    vector<CellInstance> moveCellInstanceVector;
    unordered_map<string,string> isReRouteMap;

    ifstream fin(FILEPATH);
    if (fin) {
        while (getline(fin, content)) {
            contentvector.push_back(content);
        }
    }
    fin.close();


    //TODO ??????????????????  --> ?????????
    //TODO ??????boundary map ???????????????????????? --> ?????????
    //TODO blockage ????????????????????? -> ?????????
    //TODO net supply ?????? ????????? -> ?????????
    //TODO bug case1 cell ???????????????????????? via???????????? -> ?????????
    //TODO ??????pattern route ??????????????? -> ?????????
    //TODO output file -> ?????????
    //TODO bug case3 -> ?????????
    //TODO ????????????pointer -> ?????????
    //TODO pattern route ?????? ?????? -> ?????????

    //TODO via ?????? ?????? -> ????????? case4.txt ?????????
    //TODO ?????????????????????code->

    //TODO two pin ??? cell move by ?????? ->
    //TODO random route ->
    //TODO ????????????????????????????????????????????? U - pattern route ->
    //TODO ???????????? ?????? ->
    //TODO ??????set ???????????? reduce supply??????????????? ???????????????????????? ->
    //TODO ????????????????????????reroute?????????net???weight??????????????? ->



    for (int i = 0; i < contentvector.size(); i++) {
        vector<string> lineVector = splitString(contentvector[i]);
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
            readFile.readLayer(&contentvector, &i, lineVector[1], &layerMap);
            cout << "-----layerMap end-----" << endl;
            readFile.getLayerGrid(ggridBoundaryIndex, &layerMap, &gridVector);
        } else if (lineVector[0] == NUMNONDEFAULTSUPPLYGGRID) {
            cout << "-----NumNonDefaultSupply start-----" << endl;
            readFile.readNumNonDefaultSupply(&contentvector, &gridVector, &i, lineVector[1]);
            cout << "-----NumNonDefaultSupply end-----" << endl;
        } else if (lineVector[0] == MASTERCELL) {
            cout << "-----MasterCell start-----" << endl;
            readFile.readMasterCell(&contentvector, &lineVector, &masterCellMap, &i);
            cout << "-----MasterCell end-----" << endl;
        } else if (lineVector[0] == CELLINST) {
            cout << "-----CellInstance start-----" << endl;
            readFile.readCellInstance(lineVector, &cellInstanceMap,&emptyBlockageCellVector, &masterCellMap, &blockageCellMap, &gridVector);
            cout << "-----CellInstance end-----" << endl;
        } else if (lineVector[0] == NET) {
            cout << "-----net start-----" << endl;
            readFile.readNet(&contentvector, &lineVector, &netMap, &masterCellMap, &cellInstanceMap, &netNameSet, &i);
            cout << "-----net end-----" << endl;
        } else if (lineVector[0] == NUMROUTES) {
            cout << "-----route start-----" << endl;
            readFile.readRoute(&contentvector, &lineVector, &netMap, &gridVector, &i, &reducePointMap, &netNameSet);
            cout << "-----route end-----" << endl;
        } else if (lineVector[0] == NAME) {
            cout << "-----voltage start-----" << endl;
            readFile.readVoltageArea(&contentvector, &voltageAreaMap, &i);
            cout << "-----voltage end-----" << endl;
        }
    }

    readFile.getLayerFacotr(&layerMap, &powerFactorMap);
    cellMoveRoute.cellMoveReRoute(&netMap, &cellInstanceMap, &emptyBlockageCellVector, &masterCellMap,&gridVector, &powerFactorMap,&moveCellInstanceVector,maxCellMovent,&isReRouteMap,&voltageAreaMap);

//    reRoute.boundaryReroute(&netMap, &cellInstanceMap, &masterCellMap, &gridVector, &powerFactorMap,&isReRouteMap, START);



    ofstream myfile;
    myfile.open("output_" + FILEPATH);
    myfile << "NumMovedCellInst" << " " << moveCellInstanceVector.size() << endl;
    for (const auto &cellMove : moveCellInstanceVector) {
        myfile << "CellInst" << " " << cellMove.getCellName() << " " << cellMove.getRowIndx() << " " << cellMove.getColIndx() << endl;
    };

    int numRoute = 0;
    for (const auto &net : netMap) {
        numRoute += net.second.getNumRoute().size();
    };

    myfile << "NumRoutes" << " " << numRoute << endl;
    for (const auto &net : netMap) {
        for (const auto &route : net.second.getNumRoute()) {
            myfile << route.getStartRowIndx() << " " << route.getStartColIndx() << " " << route.getStartLayIndx() << " "
                   << route.getEndRowIndx() << " " << route.getEndColIndx() << " " << route.getEndlayIndx() << " "
                   << route.getNetName() << endl;
        };
    };
    myfile.close();

    END = clock();
    cout << endl << "????????????????????????" << (double) clock() / CLOCKS_PER_SEC << " S";
    cout << endl << "?????????????????????????????????" << (END - START) / CLOCKS_PER_SEC << " S" << endl;





    //   program total wire length -> add weight -> powerFactor
//         int wire =   evaluation.wireLength(netMap);
//         cout << "wire length : " << wire << endl;
//         double score = evaluation.evaluationScore(netMap,layerMap);
//         cout << "score : " << score << endl;
//       Step 1 ????????? length ?????????boundary box??????
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






//for(const auto& item :  netMap){
//vector<Route> routVec = item.second.getNumRoute();
//for (int i =0;i<item.second.getNumRoute().size(); i++) {
//cout << routVec[i].getStartRowIndx() <<" " << routVec[i].getStartColIndx() <<" "<<  routVec[i].getStartLayIndx() <<" "<< routVec[i].getEndRowIndx()<<" " << routVec[i].getEndColIndx()<<" " << routVec[i].getEndlayIndx() <<" "<< routVec[i].getNetName() << endl;
//}
//}




//make layer grid (????????????vecotr??????????????????????????????????????????????????????)
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

//for(auto const item:voltageAreaMap){
//string voltageName =  item.first;
//cout << voltageName << endl;
//cout << "GGrid "<< endl;
//for (int i = 0; i < item.second.getGridVector().size(); ++i) {
//cout <<  item.second.getGridVector()[i].getRowIndx() << " " <<  item.second.getGridVector()[i].getColIndx() << endl;
//}
//cout << "Instance "<< endl;
//for (int i = 0; i < item.second.getInstance().size(); ++i) {
//cout << item.second.getInstance()[i] << endl;
//}
//}

//for (int i = 0; i < contentvector.size(); i++) {
//vector<string> lineVector = splitString(contentvector[i]);
//// Horizontal direction
//if(lineVector[0] == lineVector[3] and lineVector[1]!=lineVector[4] and lineVector[2] == lineVector[5]){
//
//// Vertical direction
//}else if(lineVector[0] != lineVector[3] and lineVector[1]==lineVector[4] and lineVector[2] == lineVector[5]){
//
//}else if(lineVector[0] == lineVector[3] and lineVector[1] == lineVector[4] and lineVector[2] != lineVector[5]){
//
//}else{
//cout << " wrong direction : " << lineVector[6] << endl;
//cout << lineVector[0]  << " "  <<  lineVector[1]  <<" " << lineVector[2] << " " <<lineVector[3] << " " << lineVector[4] << " " << lineVector[5]  << " " << lineVector[6]  << endl;
//}
//
//
//// Z- direction
//}


//for (int i = 0; i < contentvector.size(); i++) {
//vector<string> lineVector = splitString(contentvector[i]);
//// Horizontal direction
//if (lineVector[0] == lineVector[3] and lineVector[1] != lineVector[4] and lineVector[2] == lineVector[5]) {
//
//// Vertical direction
//} else if (lineVector[0] != lineVector[3] and lineVector[1] == lineVector[4] and
//lineVector[2] == lineVector[5]) {
//
//} else if (lineVector[0] == lineVector[3] and lineVector[1] == lineVector[4] and
//lineVector[2] != lineVector[5]) {
//
//} else {
//cout << " wrong direction : " << lineVector[6] << endl;
//cout << lineVector[0] << " " << lineVector[1] << " " << lineVector[2] << " " << lineVector[3] << " "
//<< lineVector[4] << " " << lineVector[5] << " " << lineVector[6] << endl;
//}
//// Z- direction
//}

// empty blockage
//    set<string>  masterSet;
//    for(auto const masterCell : masterCellMap){
//        int blockageDemand = 0;
//        for (auto const blockage:masterCell.second.getBlockageType()) {
//            blockageDemand+=blockage.second.getDemand();
//        }
//        if(blockageDemand == 0 and masterCell.second.getPinType().size() <=2){
////            cout << masterCell.first << " " << blockageDemand << endl;
//            masterSet.insert(masterCell.first);
//        }
//    }

//    int emptyBlockage = 0;
//    for(auto const cellInstance : cellInstanceMap){
//        if(masterSet.count(cellInstance.second.getMasterCellName()) == 1 and cellInstance.second.getMovalbe() == "Movable"){
//            emptyBlockage+=1;
//            cout << "empty blockage : "<<cellInstance.first << endl;
//        }
//    }
//    cout << "emptyBlockage : "<< emptyBlockage << endl;