//
// Created by Wu Wei shou on 2021/8/16.
//

#ifndef CELL_MOVEMENT_ROUTEPOINT_H
#define CELL_MOVEMENT_ROUTEPOINT_H

#include <iostream>
using namespace std;
class RoutePoint {
    private:
         int startPointRow;
         int startPointCol;
         int endPointRow;
         int endPointCol;
public:
    virtual ~RoutePoint();


public:
    int getStartPointRow() const;

    void setStartPointRow(int startPointRow);

    int getStartPointCol() const;

    void setStartPointCol(int startPointCol);

    int getEndPointRow() const;

    void setEndPointRow(int endPointRow);

    int getEndPointCol() const;

    void setEndPointCol(int endPointCol);
};


#endif //CELL_MOVEMENT_ROUTEPOINT_H
