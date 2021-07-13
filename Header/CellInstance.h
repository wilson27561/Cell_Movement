//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_CELLINSTANCE_H
#define CELL_MOVEMENT_CELLINSTANCE_H

#include <iostream>
#include <map>

using namespace std;
class CellInstance {
private:
        string cellName;
        int layerName;
        int rowIndx;
        int colIndx;
        string movalbe;
        map<string,string> connectPin;
        string masterCellName;
public:
    const string &getCellName() const;

    void setCellName(const string &cellName);

    const string &getMasterCellName() const;

    void setMasterCellName(const string &masterCellName);

    int getLayerName() const;

    void setLayerName(int layerName);

    int getRowIndx() const;

    void setRowIndx(int rowIndx);

    int getColIndx() const;

    void setColIndx(int colIndx);

    const string &getMovalbe() const;

    void setMovalbe(const string &movalbe);

    const map <string, string> &getConnectPin() const;

    void setConnectPin(const map <string, string> &connectPin);

};


#endif //CELL_MOVEMENT_CELLINSTANCE_H
