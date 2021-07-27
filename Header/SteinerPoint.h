//
// Created by Ｗilson on 2021/5/31.
//

#ifndef CELL_MOVEMENT_STEINERPOINT_H
#define CELL_MOVEMENT_STEINERPOINT_H

#include <iostream>
using namespace std;
class SteinerPoint {
private:
    int steinerPointRow;
    int steinerPointCol;
    int CellPointRow;
    int CellPointCol;
    int layer;
public:
    virtual ~SteinerPoint();

    SteinerPoint();


public:
    int getLayer() const;

    void setLayer(int layer);

public:
    SteinerPoint(int steinerPointRow, int steinerPointCol, int cellPointRow, int cellPointCol);

    SteinerPoint(int steinerPointRow, int steinerPointCol, int cellPointRow, int cellPointCol, int layer);


    int getSteinerPointRow() const;

    void setSteinerPointRow(int steinerPointRow);

    int getSteinerPointCol() const;

    void setSteinerPointCol(int steinerPointCol);

    int getCellPointRow() const;

    void setCellPointRow(int cellPointRow);

    int getCellPointCol() const;

    void setCellPointCol(int cellPointCol);

};


#endif //CELL_MOVEMENT_STEINERPOINT_H
