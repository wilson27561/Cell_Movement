//
// Created by Ｗilson on 2021/5/19.
//
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "../Header/Net.h"
#include "../Header/Layer.h"
using namespace std;
class Evaluation{
public:

    double evaluationScore(map<string,Net> netMap, map<string,Layer> layerMap){
//      each  Net
        double netScore =  0;
        for (const auto &item : netMap) {
           double weight = item.second.getWeight();
           double layerScore = 0;
//         each Net route
           vector<Route> routVec = item.second.getNumRoute();
//          caculate each  layer
            for (int i = 0; i < item.second.getNumRoute().size(); i++) {
//               每一層的分數要相加
               if(routVec[i].getStartLayIndx() == routVec[i].getEndlayIndx()){
                   string layName  = transferIndexToName(routVec[i].getStartLayIndx());
                   double powerFactor  = layerMap[layName].getPowerFactor();
                   double powerScore = countDistance(routVec[i])*powerFactor;
//                   cout << "Net : " << item.first << endl;
//                   cout << "layName :  "<< layName << "powerFactor : " << powerFactor << " distance : " <<  countDistance(routVec[i]) << endl;
                   layerScore+=powerScore;
               }
            }
            layerScore*=weight;
            netScore+=layerScore;
        }
        return netScore;
    }
    string transferIndexToName(int layIndex){
        string M = "M";
        return M+ to_string(layIndex);
    }

    int wireLength(map<string,Net> netMap) {
        int totalWireLength = 0;
        for (const auto &item :  netMap) {
            vector<Route> routVec = item.second.getNumRoute();
            for (int i = 0; i < item.second.getNumRoute().size(); i++) {
                totalWireLength += countDistance(routVec[i]);
            }
        }
        return totalWireLength;
    };


    int countDistance(Route route){
        if(route.getStartLayIndx() != route.getEndlayIndx()){
            return 0;
        }
        int X = route.getStartRowIndx();
        int Y = route.getStartColIndx();
        int R = route.getEndRowIndx();
        int S = route.getEndColIndx();
        int A = pow(X-R,2);
        int B = pow(Y-S,2);
        int ans = pow(A+B,0.5);
        return ans+=1;
    };
};