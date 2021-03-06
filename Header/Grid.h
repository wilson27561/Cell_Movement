//
// Created by Ｗilson on 2021/5/17.
//

#ifndef CELL_MOVEMENT_GRID_H
#define CELL_MOVEMENT_GRID_H


class Grid {
public:
    virtual ~Grid();

private:
public:
    Grid();

private:
    int rowIndx;
    int colIndx;
    int layIndx;
public:
    int getLayIndx() const;

    void setLayIndx(int layIndx);

private:
    int supply;
public:
    int getRowIndx() const;

    void setRowIndx(int rowIndx);

    int getColIndx() const;

    void setColIndx(int colIndx);

    int getSupply() const;

    void setSupply(int supply);
};


#endif //CELL_MOVEMENT_GRID_H
