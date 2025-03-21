//
// Created by nicholas on 15.03.25.
//

#include <MicrofluidicNucleation/DropletResult.h>

mfn::DropletResult::DropletResult(const double droplet_volume, const bool is_frozen, const double time_ms, const double temperature_c, bool has_volume)
{
    DropletResult::droplet_volume = droplet_volume;
    DropletResult::is_frozen = is_frozen;
    DropletResult::time_ms = time_ms;
    DropletResult::temperature_c = temperature_c;
    DropletResult::has_volume = has_volume;
}


mfn::DropletResult::DropletResult(const Frame &frame, const Droplet &droplet)
{
    DropletResult::droplet_volume = droplet.getVolume();
    DropletResult::is_frozen = droplet.isFrozen();
    DropletResult::time_ms = frame.getTime();
    DropletResult::temperature_c = frame.getTemperature();
    DropletResult::has_volume = !droplet.getIgnore();
}
