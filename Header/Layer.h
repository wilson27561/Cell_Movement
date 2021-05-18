//
// Created by Ｗilson on 2021/5/12.
//

#ifndef CELL_MOVEMENT_LAYER_H
#define CELL_MOVEMENT_LAYER_H
using namespace std;
#include <iostream>
class Layer {

    private:
//    Layer Name M1
        string layerName;
        int index;
        string routingDirection;
        int defaultSupplyOfOneGrid;
        double powerFactor;
public:
    const string &getLayerName() const;

    void setLayerName(const string &layerName);

    int getIndex() const;

    void setIndex(int index);

    const string &getRoutingDirection() const;

    void setRoutingDirection(const string &routingDirection);

    int getDefaultSupplyOfOneGrid() const;

    void setDefaultSupplyOfOneGrid(int defaultSupplyOfOneGrid);

    double getPowerFactor() const;

    void setPowerFactor(double powerFactor);
};


#endif //CELL_MOVEMENT_LAYER_H
