#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include "flute/flute.h"
#include "Util/Constant.h"
using namespace std;

int main() {

    const string filePath = "case3.txt";
    string content;
    ifstream fin(FILEPATH);
    cout << "check" << endl;
    if(fin){
        cout <<"is Existing" <<endl;
        while (fin >> content){
            cout << content << endl;
        }
    }




    fin.close();
    return 0;
}


//    int d=0;
//    int x[MAXD], y[MAXD];
//    Tree flutetree;
//    int flutewl;
//    while (!feof(stdin)) {
//        scanf("%d %d\n", &x[d], &y[d]);
//        std::cout<< "pin" << x[d]  << y[d] <<std::endl;
//        d++;
//    }
//    x[0] = 4;
//    y[0] = 1;
//    x[1] = 4;
//    y[1] = 4;
//    x[2] = 2;
//    y[2] = 2;
//    x[3] = 3;
//    y[3] = 3;
//    x[4] = 3;
//    y[4] = 1;
//    d=5;
//    readLUT();
//    flutetree = flute(d, x, y, ACCURACY);
//  std::cout<< "wirelength" << flutetree.length   <<std::endl;
//    printf("FLUTE wirelength = %d\n", flutetree.length);
//    plottree(flutetree);
//    flutewl = flute_wl(d, x, y, ACCURACY);
//    printf("FLUTE wirelength (without RSMT construction) = %d\n", flutewl);
