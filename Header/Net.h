//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_NET_H
#define CELL_MOVEMENT_NET_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include "Pin.h"
#include "Route.h"
#include "CellInstance.h"
using namespace std;
class Net {
private :


    string netName;
    int numPin;
public:
    Net();

private:
    string minRoutingConstraint;
    double weight;
    //目前沒用到
    map<string,string> connectPin;
    map<string,int> boundaryMap;
    unordered_map<string,CellInstance> connectCell;
    vector<Route> numRoute;
    int routeLength = 0;
public:
    virtual ~Net();

public:
    const map<string, string> &getConnectPin() const;

    const unordered_map<string, CellInstance> &getConnectCell() const;

    void setConnectCell(const unordered_map<string, CellInstance> &connectCell);

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


    const map<string, int> &getBoundaryMap() const;

    void setBoundaryMap(const map<string, int> &boundaryMap);

    int getRouteLength() const;

    void setRouteLength(int routeLength);
};


#endif //CELL_MOVEMENT_NET_H
