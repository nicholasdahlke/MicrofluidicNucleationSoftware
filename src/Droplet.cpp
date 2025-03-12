//
// Created by nicholas on 23.02.25.
//

#include <MicrofluidicNucleation/Droplet.h>
#include <MicrofluidicNucleation/RawDroplet.h>


mfn::Droplet::Droplet(mfn::RawDroplet droplet, double time) : RawDroplet(droplet)
{
    this->time = time;
}

double mfn::Droplet::getTime() const
{
    return time;
}

void mfn::Droplet::setTime(double time)
{
    Droplet::time = time;
}


