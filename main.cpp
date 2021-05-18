#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "flute/flute.h"
#include "Util/Constant.h"
#include "Util/Util.cpp"
#include "Util/ReadFile.cpp"
#include "Util/RoutingFunction.cpp"
#include "Header/Layer.h"
#include "Header/MasterCell.h"
#include "Header/NumNonDefaultSupplyGgrid.h"
#include "Header/CellInstance.h"
#include "Header/Net.h"
using namespace std;

void printMap(map<string,vector<NumNonDefaultSupplyGgrid>> numNonDefaultSupplyMap){
    for (auto const& item : numNonDefaultSupplyMap)
    {
        for (auto const& numNon:item.second) {
            cout  << item.first << " " << numNon.getCollndx() << " " << numNon.getRowIndx() << " "  << numNon.getLayIndx()<< " "  << numNon.getIncrOrDecrValue() <<endl;
        }
    }
}


int main() {

    Util util;
    ReadFile readFile;
    RoutingFunction routingFunction;
    const string filePath = "case2.txt";
    string content;
    vector<string> contentvector;
    ifstream fin(FILEPATH);
//   case Data
    int maxCellMovent = 0;
    GgridBoundaryIndex ggridBoundaryIndex;
    map<string,Layer> layerMap;
    map<string,vector<NumNonDefaultSupplyGgrid>> numNonDefaultSupplyMap;
    map<string,MasterCell> masterCellMap;
    map<string,CellInstance> cellInstanceMap;
    map<string,VoltageArea> voltageAreaMap;
    map<string,Net> netMap;

    if(fin){
        while (getline(fin,content)){
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
            numNonDefaultSupplyMap = readFile.readNumNonDefaultSupply(contentvector, numNonDefaultSupplyMap, i,lineVector[1]);
//             printMap(numNonDefaultSupplyMap);
        } else if (lineVector[0] == MASTERCELL) {
            masterCellMap = readFile.readMasterCell(contentvector, lineVector, masterCellMap, i);
        } else if (lineVector[0] == CELLINST) {
            cellInstanceMap = readFile.readCellInstance(lineVector, cellInstanceMap);
        } else if (lineVector[0] == NET) {
            netMap = readFile.readNet(contentvector, lineVector, netMap, i);
        } else if (lineVector[0] == NUMROUTES) {
            netMap = readFile.readRoute(contentvector, lineVector, netMap, i);
        } else if (lineVector[0] == NUMVOLTAGEAREA) {
            voltageAreaMap = readFile.readVoltageArea(contentvector, lineVector, voltageAreaMap, i);
        };
    }

    //         program total wire length -> add weight -> powerFactor
         int wire =   routingFunction.wireLength(netMap);
        cout << "wirelength : " << wire << endl;

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