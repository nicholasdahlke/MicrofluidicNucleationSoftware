//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_ANALYSISCONFIG_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_ANALYSISCONFIG_H
#include <filesystem>
namespace mfn
{
    class AnalysisConfig
    {
    public:
        bool show_frames_droplets = false;
        bool show_frames_displacement = false;
        bool show_displacement_vectors = false;
        int right_border_displacement = 1e6;
        float max_movement_threshold_displacement = 1e4;
        int left_border_volume = 5;
        int right_border_volume = 1e4;
        int x_threshold_count = 20;
        double calib = 1.0;
        float score_threshold = 0.5;
        float nms_threshold = 0.4;
        float confidence_threshold = 0.45;
        float speed_border_left = 50;
        float speed_border_right = 50;
        std::filesystem::path net_file;
        int skip_frames_volume = 0;
        int frame_start = 0;
        int frame_stop = 0;
        int parallel = 1;
    };
}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_ANALYSISCONFIG_H

