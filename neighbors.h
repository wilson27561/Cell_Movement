//
// Created by Ｗilson on 2021/4/27.
//

#ifndef CELL_MOVEMENT_NEIGHBORS_H
#define CELL_MOVEMENT_NEIGHBORS_H

#include "global.h"

void  allocate_nn_arrays( long n );
void  deallocate_nn_arrays();

void  brute_force_nearest_neighbors
        (
                long       n,
                Point*     pt,
                nn_array*  nn
        );

void  dq_nearest_neighbors
        (
                long       n,
                Point*     pt,
                nn_array*  nn
        );


#endif //CELL_MOVEMENT_NEIGHBORS_H
