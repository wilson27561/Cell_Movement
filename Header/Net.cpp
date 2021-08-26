//
// Created by Ｗilson on 2021/5/17.
//

#include "Net.h"

const string &Net::getNetName() const {
    return netName;
}

void Net::setNetName(const string &netName) {
    Net::netName = netName;
}

int Net::getNumPin() const {
    return numPin;
}

void Net::setNumPin(int numPin) {
    Net::numPin = numPin;
}

const string &Net::getMinRoutingConstraint() const {
    return minRoutingConstraint;
}

void Net::setMinRoutingConstraint(const string &minRoutingConstraint) {
    Net::minRoutingConstraint = minRoutingConstraint;
}

double Net::getWeight() const {
    return weight;
}

void Net::setWeight(double weight) {
    Net::weight = weight;
}


const map<string, string> &Net::getConnectPin() const {
    return connectPin;
}

void Net::setConnectPin(const map<string, string> &connectPin) {
    Net::connectPin = connectPin;
}

const vector<Route> &Net::getNumRoute() const {
    return numRoute;
}

void Net::setNumRoute(const vector<Route> &numRoute) {
    Net::numRoute = numRoute;
}



const map<string, int> &Net::getBoundaryMap() const {
    return boundaryMap;
}

void Net::setBoundaryMap(const map<string, int> &boundaryMap) {
    Net::boundaryMap = boundaryMap;
}

Net::~Net() {

}

Net::Net() {}

const unordered_map<string, CellInstance> &Net::getConnectCell() const {
    return connectCell;
}

void Net::setConnectCell(const unordered_map<string, CellInstance> &connectCell) {
    Net::connectCell = connectCell;
}
