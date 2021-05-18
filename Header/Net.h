//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_NET_H
#define CELL_MOVEMENT_NET_H

#include <iostream>
#include <vector>
#include <map>
#include "Pin.h"
#include "Route.h"
using namespace std;
class Net {
private :
    string netName;
    int numPin;
    string minRoutingConstraint;
    double weight;
    map<string,string> connectPin;
    vector<Route> numRoute;
public:
    const map<string, string> &getConnectPin() const;

    void setConnectPin(const map<string, string> &connectPin);

public:
    const string &getNetName() const;

    void setNetName(const string &netName);

    int getNumPin() const;

    void setNumPin(int numPin);

    const string &getMinRoutingConstraint() const;

    void setMinRoutingConstraint(const string &minRoutingConstraint);

    double getWeight() const;

    void setWeight(double weight);

    const vector<Route> &getNumRoute() const;

    void setNumRoute(const vector<Route> &numRoute);


};


#endif //CELL_MOVEMENT_NET_H
