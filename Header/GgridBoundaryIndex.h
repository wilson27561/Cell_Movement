//
// Created by Ｗilson on 2021/5/15.
//

#ifndef CELL_MOVEMENT_GGRIDBOUNDARYINDEX_H
#define CELL_MOVEMENT_GGRIDBOUNDARYINDEX_H


class GgridBoundaryIndex {
public:
    int getRowBeginIdx() const;

    void setRowBeginIdx(int rowBeginIdx);

    int getColBeginIdx() const;

    void setColBeginIdx(int colBeginIdx);

    int getRowEndIdx() const;

    void setRowEndIdx(int rowEndIdx);

    int getColEndIdx() const;

    void setColEndIdx(int colEndIdx);

private :
    int rowBeginIdx;
    int colBeginIdx;
    int rowEndIdx;
    int colEndIdx;
};


#endif //CELL_MOVEMENT_GGRIDBOUNDARYINDEX_H
