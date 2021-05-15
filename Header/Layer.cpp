//
// Created by Ｗilson on 2021/5/12.
//

#include "Layer.h"

const string &Layer::getLayerName() const {
    return layerName;
}

void Layer::setLayerName(const string &layerName) {
    Layer::layerName = layerName;
}

int Layer::getIndex() const {
    return index;
}

void Layer::setIndex(int index) {
    Layer::index = index;
}

const string &Layer::getRoutingDirection() const {
    return routingDirection;
}

void Layer::setRoutingDirection(const string &routingDirection) {
    Layer::routingDirection = routingDirection;
}

int Layer::getDefaultSupplyOfOneGrid() const {
    return defaultSupplyOfOneGrid;
}

void Layer::setDefaultSupplyOfOneGrid(int defaultSupplyOfOneGrid) {
    Layer::defaultSupplyOfOneGrid = defaultSupplyOfOneGrid;
}

double Layer::getPowerFactor() const {
    return powerFactor;
}

void Layer::setPowerFactor(double powerFactor) {
    Layer::powerFactor = powerFactor;
}
