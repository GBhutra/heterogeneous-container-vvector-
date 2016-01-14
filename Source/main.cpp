//
//  main.cpp
//  project_
//
//  Created by Ghanshyam Bhutra and Siddhant Rath on 11/17/15.
//  Copyright © 2015 Ghanshyam Bhutra. All rights reserved.
//
#include <iostream>
#include "vvector.h"
#include "polyObject.h"
/*
less_than_comparable
equality_comparable
addable
ostreamable
*/

int main() {
     vvector<ostreamable,equality_comparable,addable> V;
     int i1=5,i2=5;
     char c='s';
     std::string str = "I am a variable vector";
     V.push_back(i1);
     V.push_back(i2);
     if (V[0]==V[1])
     V.push_back(c);
     V.push_back(str);
     for(auto v: V)	{
         std::cout<<"\t"<<v;
     }
    std::cout<<"\n";
    return 0;
}



