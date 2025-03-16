//
// Created by nicholas on 16.03.25.
//

#ifndef MICROCALIBRATOR_H
#define MICROCALIBRATOR_H
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <filesystem>

namespace mfngui
{
    class MicroCalibrator {
    public:
        MicroCalibrator(std::filesystem::path _video_path, int _frame=0);
        double getCalibrationConstant();

    private:

        cv::Mat preview_image_current;
        cv::Mat preview_image_original;
        double distance_px = 0.0;
        double distance_mm = 0.0;
        cv::RotatedRect tubeRect;
        int rect_angle = 0;
        int rect_rotation = 0;
        int rect_height;
        int rect_width;
        int rect_translation_y;

        int frame_width;
        int frame_height;

        void redrawImage();
        static void rotation_tr_callback(int value, void* data);
        static void translation_y_tr_callback(int value, void* data);
        static void height_tr_callback(int value, void* data);

        static cv::RotatedRect makeRect(MicroCalibrator * calibrator);
        void showDistancePopup();
    };
}


#endif //MICROCALIBRATOR_H
