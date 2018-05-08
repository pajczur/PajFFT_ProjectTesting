/*
  ==============================================================================

    Clock.h
    Created: 13 Mar 2018 9:38:37pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <stdio.h>

using namespace std;

class Clock
{
public:
    Clock();
    ~Clock();
    
    float secondsElapsed();
    void  reset();
    
private:
    chrono::time_point<chrono::high_resolution_clock> start;
};


//    Clock _time;
//    std::cout << _time.secondsElapsed() << std::endl;
