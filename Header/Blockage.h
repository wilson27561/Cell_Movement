//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_BLOCKAGE_H
#define CELL_MOVEMENT_BLOCKAGE_H

#include <iostream>
using namespace std;
class Blockage {
private:
    string blockageName;
    string blockageLayer;
    int demand;
public:
    const string &getBlockageName() const;

    void setBlockageName(const string &blockageName);

    const string &getBlockageLayer() const;

    void setBlockageLayer(const string &blockageLayer);

    int getDemand() const;

    void setDemand(int demand);

};


#endif //CELL_MOVEMENT_BLOCKAGE_H
