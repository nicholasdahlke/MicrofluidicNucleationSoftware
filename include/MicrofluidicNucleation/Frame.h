//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_FRAME_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_FRAME_H
#include <vector>
#include <MicrofluidicNucleation/RawDroplet.h>


namespace mfn
{
    class Frame
    {
    public:
        std::vector<mfn::RawDroplet> droplets;

        explicit Frame(double time);
        double getTime() const;

    private:
        double time;
    };
}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_FRAME_H
