//
//  userobject.cpp
//  Project1
//
//  Created by Lee Robertson on 10/13/18.
//  Copyright © 2018 Lee Robertson. All rights reserved.
//

#include <cstdlib>
#include <string>
using namespace std;

class userobject {
public:
    string username;
    string accesslevel;
    int temp;
    
    userobject(string uname = " " ,string al = " "){ //users constructor
        this->username = uname;
        this->accesslevel = al;
        this->temp = 0;
    }
    
};
