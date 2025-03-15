//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_FRAME_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_FRAME_H
#include <vector>
#include <MicrofluidicNucleation/Droplet.h>


namespace mfn
{
    class Frame
    {
    public:
        std::vector<mfn::Droplet> droplets;

        explicit Frame(double time);
        double getTime() const;
        float getTemperature() const;
        void setTemperature(float temperature_k);

    private:
        float temperature_k{};
        double time;
    };
}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_FRAME_H
