//
// Created by Wu Wei shou on 2021/8/16.
//

#ifndef CELL_MOVEMENT_PATTERNPOINT_H
#define CELL_MOVEMENT_PATTERNPOINT_H

#include <iostream>
#include <vector>
#include <iostream>
using namespace std;
class PatternPoint {
private:
    int startPointRow;
    int startPointCol;
    int endPointRow;
    int endPointCol;
    int layer;
public:
    PatternPoint();

    virtual ~PatternPoint();


public:
    int getStartPointRow() const;

    void setStartPointRow(int startPointRow);

    int getStartPointCol() const;

    void setStartPointCol(int startPointCol);

    int getEndPointRow() const;

    void setEndPointRow(int endPointRow);

    int getEndPointCol() const;

    void setEndPointCol(int endPointCol);

    int getLayer() const;

    void setLayer(int layer);


};


#endif //CELL_MOVEMENT_PATTERNPOINT_H
