//
// Created by Ｗilson on 2021/5/18.
//

#include <iostream>
#include <map>
#include <math.h>
#include "../Header/Net.h"
using namespace std;
class RoutingFunction{
public :

    double EvaluationScore(){


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
        int X = route.getStartLayIndx();
        int Y = route.getStartColIndx();
        int R = route.getEndRowIndx();
        int S = route.getEndColIndx();
        int A = pow(X-R,2);
        int B = pow(Y-S,2);
        int ans = pow(A+B,0.5);
        return ans;
    };
};


//        for (const auto &item :  netMap) {
//            vector<Route> routVec = item.second.getNumRoute();
//            for (int i = 0; i < item.second.getNumRoute().size(); i++) {
//                cout << routVec[i].getStartRowIndx() << " " << routVec[i].getStartColIndx() << " "
//                     << routVec[i].getStartLayIndx() << " " << routVec[i].getEndRowIndx() << " "
//                     << routVec[i].getEndColIndx() << " " << routVec[i].getEndlayIndx() << " "
//                     << routVec[i].getNetName() << endl;
//            }
//        }