//
// Created by Wu Wei shou on 2021/8/16.
//

#include "RoutePoint.h"

int RoutePoint::getStartPointRow() const {
    return startPointRow;
}

void RoutePoint::setStartPointRow(int startPointRow) {
    RoutePoint::startPointRow = startPointRow;
}

int RoutePoint::getStartPointCol() const {
    return startPointCol;
}

void RoutePoint::setStartPointCol(int startPointCol) {
    RoutePoint::startPointCol = startPointCol;
}

int RoutePoint::getEndPointRow() const {
    return endPointRow;
}

void RoutePoint::setEndPointRow(int endPointRow) {
    RoutePoint::endPointRow = endPointRow;
}

int RoutePoint::getEndPointCol() const {
    return endPointCol;
}

void RoutePoint::setEndPointCol(int endPointCol) {
    RoutePoint::endPointCol = endPointCol;
}

RoutePoint::~RoutePoint() {

}
