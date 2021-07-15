//
// Created by Ｗilson on 2021/5/15.
//
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;
class Util {

public :
    vector<string> splitString( string content) //MasterCell MC1 2 2
    {
        int i = 0;
        vector<string> contentVector;
        stringstream contentArray(content);
        string word;
        while (contentArray.good())
        {
            contentArray >> word ;
            contentVector.push_back(word);
            ++i;
        }
        return contentVector;
    }
};