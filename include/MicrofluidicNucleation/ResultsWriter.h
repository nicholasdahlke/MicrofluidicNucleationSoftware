//
// Created by nicholas on 14.03.25.
//

#ifndef RESULTSWRITER_H
#define RESULTSWRITER_H
#include <MicrofluidicNucleation/VideoAnalyzer.h>
#include <MicrofluidicNucleation/DropletResult.h>
#include <MicrofluidicNucleation/Experiment.h>
namespace mfn
{
    class ResultsWriter
    {
    public:
        explicit ResultsWriter(const VideoAnalyzer& videoAnalyzer, const Experiment &experiment);
        std::vector<mfn::DropletResult> getDropletResults() const;
        std::vector<double> getSpeeds() const;
        std::vector<double> getVolumes() const;


    private:
        mfn::VideoAnalyzer videoAnalyzer;
        mfn::Experiment experiment;

    };
}

#endif //RESULTSWRITER_H
