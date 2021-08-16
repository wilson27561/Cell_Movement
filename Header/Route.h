//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_ROUTE_H
#define CELL_MOVEMENT_ROUTE_H

#include <iostream>

using namespace std;

class Route {
private:
    string netName;
    int startRowIndx;
    int startColIndx;
    int startLayIndx;
    int endRowIndx;
    int endColIndx;
    int endlayIndx;
public:
    Route();

    virtual ~Route();

public:

    const string &getNetName() const;

    void setNetName(const string &netName);

    int getStartRowIndx() const;

    void setStartRowIndx(int startRowIndx);

    int getStartColIndx() const;

    void setStartColIndx(int startColIndx);

    int getStartLayIndx() const;

    void setStartLayIndx(int startLayIndx);

    int getEndRowIndx() const;

    void setEndRowIndx(int endRowIndx);

    int getEndColIndx() const;

    void setEndColIndx(int endColIndx);

    int getEndlayIndx() const;

    void setEndlayIndx(int endlayIndx);


};


#endif //CELL_MOVEMENT_ROUTE_H
