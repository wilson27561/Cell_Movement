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
    int blockageLayer;
public:
    int getBlockageLayer() const;

    void setBlockageLayer(int blockageLayer);

private:
    int demand;
public:
    const string &getBlockageName() const;

    void setBlockageName(const string &blockageName);


    int getDemand() const;

    void setDemand(int demand);

};


#endif //CELL_MOVEMENT_BLOCKAGE_H
