//
// Created by Ｗilson on 2021/4/27.
//

#ifndef CELL_MOVEMENT_MST2_H
#define CELL_MOVEMENT_MST2_H


#include "global.h"

void  mst2_package_init( long  n );
void  mst2_package_done();
void  mst2( long n, Point* pt, long* parent );

#endif
