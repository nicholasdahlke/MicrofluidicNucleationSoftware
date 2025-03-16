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

        explicit Frame(double time_ms);
        double getTime() const;
        float getTemperature() const;
        void setTemperature(float temperature_c);

    private:
        float temperature_c{};
        double time_ms;
    };
}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_FRAME_H
