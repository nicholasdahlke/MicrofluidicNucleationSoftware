#include "MicrofluidicNucleation/Frame.h"

//
// Created by nicholas on 23.02.25.
//
double mfn::Frame::getTime() const
{
    return time;
}

mfn::Frame::Frame(const double time)
{
    Frame::time = time;
}

void mfn::Frame::setTemperature(float temperature_k)
{
    this->temperature_k = temperature_k;
}

float mfn::Frame::getTemperature() const
{
    return temperature_k;
}