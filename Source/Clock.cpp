/*
  ==============================================================================

    Clock.cpp
    Created: 13 Mar 2018 9:38:37pm
    Author:  Wojtek Pilwinski

  ==============================================================================
*/

#include "Clock.h"

Clock::Clock()
{
    start = chrono::high_resolution_clock::now();
}

Clock::~Clock()
{
}

float Clock::secondsElapsed()
{
    auto stop = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(stop - start).count();
}


void Clock::reset()
{
    start = chrono::high_resolution_clock::now();
}
