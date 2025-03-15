//
// Created by nicholas on 14.03.25.
//

#ifndef RESULTSWRITER_H
#define RESULTSWRITER_H
#include <MicrofluidicNucleation/VideoAnalyzer.h>
namespace mfn
{
    class ResultsWriter
    {
    public:
        explicit ResultsWriter(const VideoAnalyzer& videoAnalyzer);
        std::tuple<int, int> countDroplets() const;


    private:
        VideoAnalyzer videoAnalyzer;

    };
}

#endif //RESULTSWRITER_H
