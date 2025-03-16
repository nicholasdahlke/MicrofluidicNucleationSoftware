//
// Created by nicholas on 15.03.25.
//

#ifndef DROPLETRESULT_H
#define DROPLETRESULT_H
#include <MicrofluidicNucleation/Frame.h>
#include <MicrofluidicNucleation/Droplet.h>

namespace mfn
{
    class DropletResult
    {
    public:
        double droplet_volume;
        bool is_frozen;
        double time_ms;
        double temperature_c;

        DropletResult() = default;
        DropletResult(double droplet_volume, bool is_frozen, double time_ms, double temperature_c);
        DropletResult(const Frame& frame, const Droplet& droplet);

    };
}
#endif //DROPLETRESULT_H

