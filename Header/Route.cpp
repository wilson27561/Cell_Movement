//
// Created by Ｗilson on 2021/5/17.
//

#include "Route.h"

const string &Route::getNetName() const {
    return netName;
}

void Route::setNetName(const string &netName) {
    Route::netName = netName;
}

int Route::getStartRowIndx() const {
    return startRowIndx;
}

void Route::setStartRowIndx(int startRowIndx) {
    Route::startRowIndx = startRowIndx;
}

int Route::getStartColIndx() const {
    return startColIndx;
}

void Route::setStartColIndx(int startColIndx) {
    Route::startColIndx = startColIndx;
}

int Route::getStartLayIndx() const {
    return startLayIndx;
}

void Route::setStartLayIndx(int startLayIndx) {
    Route::startLayIndx = startLayIndx;
}

int Route::getEndRowIndx() const {
    return endRowIndx;
}

void Route::setEndRowIndx(int endRowIndx) {
    Route::endRowIndx = endRowIndx;
}

int Route::getEndColIndx() const {
    return endColIndx;
}

void Route::setEndColIndx(int endColIndx) {
    Route::endColIndx = endColIndx;
}

int Route::getEndlayIndx() const {
    return endlayIndx;
}

void Route::setEndlayIndx(int endlayIndx) {
    Route::endlayIndx = endlayIndx;
}

