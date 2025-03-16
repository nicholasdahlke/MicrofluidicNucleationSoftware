#include "MicrofluidicNucleation/Frame.h"

//
// Created by nicholas on 23.02.25.
//
double mfn::Frame::getTime() const
{
    return time_ms;
}

mfn::Frame::Frame(const double time_ms)
{
    Frame::time_ms = time_ms;
}

void mfn::Frame::setTemperature(float temperature_c)
{
    this->temperature_c = temperature_c;
}

float mfn::Frame::getTemperature() const
{
    return temperature_c;
}