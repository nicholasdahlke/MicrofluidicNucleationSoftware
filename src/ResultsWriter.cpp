//
// Created by nicholas on 14.03.25.
//

#include <MicrofluidicNucleation/ResultsWriter.h>
#include <MicrofluidicNucleation/Vector2D.h>
#include <cmath>

// Calculates global parameters for the run
mfn::ResultsWriter::ResultsWriter(const VideoAnalyzer &videoAnalyzer, const Experiment &experiment)
{
    this->videoAnalyzer = videoAnalyzer;
    this->experiment = experiment;
}

std::vector<mfn::DropletResult> mfn::ResultsWriter::getDropletResults() const
{
    std::vector<mfn::DropletResult> results;
    for (const Frame & frame : videoAnalyzer.getFrames())
    {
        for (const Droplet& droplet : frame.droplets)
        {
            results.emplace_back(droplet.getVolume()*pow(experiment.getParameter("calibration"), 3), droplet.isFrozen(), frame.getTime(), frame.getTemperature(), !droplet.getIgnore());
        }
    }
    return results;
}

std::vector<double> mfn::ResultsWriter::getSpeeds() const
{
    std::vector<double> speeds;
    for (const Frame & frame : videoAnalyzer.getFrames())
    {
        for (const Droplet & droplet : frame.droplets)
        {
            Vector2D movement = droplet.getMovement();
            double calibration = experiment.getParameter("calibration");
            movement.content[0] *= calibration;
            movement.content[1] *= calibration;
            speeds.push_back(movement.get_length()*experiment.getParameter("frame_rate"));
        }
    }
    return speeds;
}

std::vector<double> mfn::ResultsWriter::getVolumes() const
{
    std::vector<double> volumes;
    for (const DropletResult & droplet : videoAnalyzer.getDropletHeap())
    {
        if (droplet.has_volume)
            volumes.push_back(droplet.droplet_volume * pow(experiment.getParameter("calibration"), 3));
    }
    return volumes;
}