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

const vector<CellInstance> &Net::getConnectCell() const {
    return connectCell;
}

void Net::setConnectCell(const vector<CellInstance> &connectCell) {
    Net::connectCell = connectCell;
}
