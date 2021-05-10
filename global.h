//
// Created by Ｗilson on 2021/4/27.
//

#ifndef CELL_MOVEMENT_GLOBAL_H
#define CELL_MOVEMENT_GLOBAL_H

#include <stdio.h>

#define  TRUE   1
#define  FALSE  0
#define  MAXLONG  0x7fffffffL

struct point
{
    long    x, y;
};

typedef  struct point Point;

typedef  long nn_array[8];

#endif //CELL_MOVEMENT_GLOBAL_H
