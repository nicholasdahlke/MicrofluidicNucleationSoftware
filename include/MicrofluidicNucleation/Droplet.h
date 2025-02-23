//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_DROPLET_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_DROPLET_H
#include <MicrofluidicNucleation/RawDroplet.h>

namespace mfn
{
    class Droplet : public RawDroplet
    {
    public:
        Droplet(RawDroplet droplet, double time);

        double getTime() const;

        void setTime(double time);


    private:
        double time;
    };
}

#endif //MICROFLUIDICNUCLEATIONSOFTWARE_DROPLET_H
