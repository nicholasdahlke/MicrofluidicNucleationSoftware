//
// Created by nicholas on 16.03.25.
//

#include <MicroCalibrator/MicroCalibrator.h>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>

mfngui::MicroCalibrator::MicroCalibrator(std::filesystem::path _video_path, int _frame)
{
    cv::VideoCapture _video(_video_path);
    _video.set(cv::CAP_PROP_FRAME_COUNT, _frame);
    _video.read(preview_image_current);
    frame_height = _video.get(cv::CAP_PROP_FRAME_HEIGHT);
    frame_width = _video.get(cv::CAP_PROP_FRAME_WIDTH);
    rect_height = frame_height/3;
    rect_width = frame_width/3;
    rect_translation_y = frame_height/2;
    preview_image_original = preview_image_current.clone();
}

void mfngui::MicroCalibrator::rotation_tr_callback(int value, void *data)
{
    MicroCalibrator * calibrator = (MicroCalibrator*)data;
    calibrator->rect_angle = value;
    calibrator->tubeRect = makeRect(calibrator);
    calibrator->redrawImage();
}

void mfngui::MicroCalibrator::translation_y_tr_callback(int value, void *data)
{
    MicroCalibrator * calibrator = (MicroCalibrator*)data;
    calibrator->rect_translation_y = value;
    calibrator->tubeRect = makeRect(calibrator);
    calibrator->redrawImage();
}

void mfngui::MicroCalibrator::height_tr_callback(int value, void *data)
{
    MicroCalibrator * calibrator = (MicroCalibrator*)data;
    calibrator->rect_height = value;
    calibrator->distance_px = value;
    calibrator->tubeRect = makeRect(calibrator);
    calibrator->redrawImage();
}

cv::RotatedRect mfngui::MicroCalibrator::makeRect(MicroCalibrator *calibrator)
{
    return cv::RotatedRect(cv::Point2f(calibrator->frame_width/2, calibrator->rect_translation_y), cv::Size2f(calibrator->rect_height, calibrator->rect_width), calibrator->rect_angle);
}

void mfngui::MicroCalibrator::redrawImage()
{
    preview_image_current = preview_image_original.clone();
    // Drawing Code here
    if (rect_width > 0 && rect_height > 0)
    {
        cv::Point2f vertices[4];
        tubeRect.points(vertices);
        for(size_t i = 0; i<4; i++)
        {
            cv::line(preview_image_current, vertices[i], vertices[(i+1)%4], cv::Scalar(0, 139, 237),2);
        }
    }
}

void mfngui::MicroCalibrator::showDistancePopup()
{
    char distance_real_cstr[1024];
    FILE *fp = popen("zenity  --title  \"Distance\" --entry --text \"Enter distance in mm here\"", "r");
    fgets(distance_real_cstr, 1024, fp);
    std::string distance_real_str(distance_real_cstr);
    float distance_real = std::stof(distance_real_str);
    distance_mm = distance_real;
    std::cout << distance_mm;
}

double mfngui::MicroCalibrator::getCalibrationConstant()
{
    std::string window_name = "Calibration Frame";
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
    cv::createTrackbar("Rotation", window_name, NULL, 360, rotation_tr_callback,this);
    cv::createTrackbar("Translation: y", window_name, NULL, frame_height, translation_y_tr_callback, this);
    cv::createTrackbar("Height", window_name, NULL, frame_height, height_tr_callback, this);
    for(;;)
    {
        cv::imshow(window_name, preview_image_current);
        int key = cv::waitKey(20);
        if(key == 13)
        {
            showDistancePopup();
            break;
        }
    }
    cv::destroyWindow(window_name);
    return (distance_mm / distance_px)/1000;
}