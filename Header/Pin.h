//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_PIN_H
#define CELL_MOVEMENT_PIN_H

#include <iostream>
using namespace std;
class Pin {
 private:
    string pinName;
    bool isConnect;
    string layer;
    string connectCell;
public:
    const string &getPinName() const;

    void setPinName(const string &pinName);

    bool isConnect1() const;

    void setIsConnect(bool isConnect);

    const string &getLayer() const;

    void setLayer(const string &layer);

    const string &getConnectCell() const;

    void setConnectCell(const string &connectCell);

};


#endif //CELL_MOVEMENT_PIN_H
