//
// Created by Ｗilson on 2021/5/16.
//

#ifndef CELL_MOVEMENT_NUMNONDEFAULTSUPPLYGGRID_H
#define CELL_MOVEMENT_NUMNONDEFAULTSUPPLYGGRID_H
#include <iostream>
using namespace std;
class NumNonDefaultSupplyGgrid {
private :
    int rowIndx;
    int collndx;
    int layIndx;
    string layerName;
    int incrOrDecrValue;
public:
    virtual ~NumNonDefaultSupplyGgrid();

    NumNonDefaultSupplyGgrid();

public:
    int getRowIndx() const;

    void setRowIndx(int rowIndx);

    int getCollndx() const;

    void setCollndx(int collndx);

    int getLayIndx() const;

    void setLayIndx(int layIndx);

    const string &getLayerName() const;

    void setLayerName(const string &layerName);

    int getIncrOrDecrValue() const;

    void setIncrOrDecrValue(int incrOrDecrValue);

};


#endif //CELL_MOVEMENT_NUMNONDEFAULTSUPPLYGGRID_H
