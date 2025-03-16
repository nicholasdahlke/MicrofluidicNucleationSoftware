//
// Created by nicholas on 14.03.25.
//

#include <MicrofluidicNucleation/ResultsWriter.h>

mfn::ResultsWriter::ResultsWriter(const VideoAnalyzer &videoAnalyzer)
{
    this->videoAnalyzer = videoAnalyzer;
}

std::tuple<int, int> mfn::ResultsWriter::countDroplets() const
{
    int frozen_count = 0;
    int liquid_count = 0;

    for (const Frame & frame : videoAnalyzer.getFrames())
    {
        for (const Droplet & droplet : frame.droplets)
        {
            if (droplet.isFrozen())
            {
                frozen_count++;
            }
            else
            {
                liquid_count++;
            }
        }
    }
    return std::make_tuple(frozen_count, liquid_count);
}

std::vector<mfn::DropletResult> mfn::ResultsWriter::getDropletResults() const
{
    std::vector<mfn::DropletResult> results;
    for (const Frame & frame : videoAnalyzer.getFrames())
    {
        for (const Droplet & droplet : frame.droplets)
            results.emplace_back(frame, droplet);
    }
    return results;
}