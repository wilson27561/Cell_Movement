//
// Created by Ｗilson on 2021/5/17.
//

#include "Pin.h"

const string &Pin::getPinName() const {
    return pinName;
}

void Pin::setPinName(const string &pinName) {
    Pin::pinName = pinName;
}

bool Pin::isConnect1() const {
    return isConnect;
}

void Pin::setIsConnect(bool isConnect) {
    Pin::isConnect = isConnect;
}

const string &Pin::getLayer() const {
    return layer;
}

void Pin::setLayer(const string &layer) {
    Pin::layer = layer;
}

const string &Pin::getConnectCell() const {
    return connectCell;
}

void Pin::setConnectCell(const string &connectCell) {
    Pin::connectCell = connectCell;
}
