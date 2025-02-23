#include "MicrofluidicNucleation/Frame.h"

//
// Created by nicholas on 23.02.25.
//
double mfn::Frame::getTime() const
{
    return time;
}

mfn::Frame::Frame(double time)
{
    Frame::time = time;
}