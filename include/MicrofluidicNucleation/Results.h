//
// Created by Nicholas Dahlke on 25.02.2025.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_RESULTS_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_RESULTS_H
#include <MicrofluidicNucleation/Droplet.h>
#include <vector>

namespace mfn
{
    class Results
    {
    public:
        std::vector<mfn::Droplet> droplets;
    };
}

#endif

//End of file Results.h
