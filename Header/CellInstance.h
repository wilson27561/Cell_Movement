//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_CELLINSTANCE_H
#define CELL_MOVEMENT_CELLINSTANCE_H

#include <iostream>
using namespace std;
class CellInstance {
private:
        string cellName;
        string layerName;
        int rowIndx;
        int colIndx;
        string movalbe;
        string connectPin;
        string masterCellName;
public:
    const string &getCellName() const;

    void setCellName(const string &cellName);

    const string &getMasterCellName() const;

    void setMasterCellName(const string &masterCellName);

    const string &getLayerName() const;

    void setLayerName(const string &layerName);

    int getRowIndx() const;

    void setRowIndx(int rowIndx);

    int getColIndx() const;

    void setColIndx(int colIndx);

    const string &getMovalbe() const;

    void setMovalbe(const string &movalbe);

    const string &getConnectPin() const;

    void setConnectPin(const string &connectPin);

};


#endif //CELL_MOVEMENT_CELLINSTANCE_H
