//
// Created by nicholas on 25.02.25.
//

#include <MicrofluidicNucleation/YoloV5.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include <opencv2/imgproc.hpp>

mfn::YoloV5::YoloV5(const mfn::AnalysisConfig &config)
{
    open(config);
}

void mfn::YoloV5::open(const mfn::AnalysisConfig &config)
{
    if(!std::filesystem::exists(config.net_file))
        spdlog::get("mfn_logger")->error("An error occurred opening the neural net file.");
    else
        yolo = cv::dnn::readNet(config.net_file);

    net_input_size = cv::Size(640.0, 640.0);
    YoloV5::config = config;
}

std::vector<mfn::Detection> mfn::YoloV5::process(const cv::Mat &input)
{
    if(yolo.empty())
        spdlog::get("mfn_logger")->error("Error during inference. Net empty");

    spdlog::get("mfn_logger")->info("Generating blob");
    cv::Mat blob = cv::dnn::blobFromImage(input,
                                          1.0/255.0,
                                          net_input_size,
                                          cv::Scalar(0, 0, 0),
                                          true,
                                          false);

    spdlog::get("mfn_logger")->info("Starting inference");
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::vector<cv::Mat> outputs;
    yolo.setInput(blob);
    yolo.forward(outputs, yolo.getUnconnectedOutLayersNames());
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    spdlog::get("mfn_logger")->info("Inference finished in {}ms", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

    return getDetectionFromOutput(input, outputs);
}

std::vector<mfn::Detection> mfn::YoloV5::getDetectionFromOutput(const cv::Mat &input, const std::vector<cv::Mat> &outputs) const
{
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    std::vector<std::string> class_names;
    class_names.emplace_back("droplets");
    class_names.emplace_back("droplets_frozen");

    float x_factor = input.cols / net_input_size.width;
    float y_factor = input.rows / net_input_size.height;
    auto data = (float *)outputs[0].data;
    const int rows = 25200;
    const int dimensions = 7; // Always adjust for multi class models

    for (size_t i = 0; i < rows; ++i)
    {
        float confidence = data[4];
        if(confidence >= config.confidence_threshold)
        {
            float * classes_score = data + 5;
            cv::Mat scores(1, class_names.size(), CV_32FC1, classes_score);
            cv::Point class_id;
            double max_class_score;
            cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
            if(max_class_score > config.score_threshold)
            {
                confidences.push_back(confidence);
                class_ids.push_back(class_id.x);

                float cx = data[0];
                float cy = data[1];

                float w = data[2];
                float h = data[3];

                int left = int((cx - 0.5 * w) * x_factor);
                int top = int((cy - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);

                boxes.emplace_back(cv::Point(left, top), cv::Point(left + width, top + height));
            }
        }
        data += dimensions;
    }
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, config.score_threshold, config.nms_threshold, indices);
    std::vector<Detection> detections;
    for (int idx : indices)
    {
        cv::Rect box = boxes[idx];
        std::string label = cv::format("%.2f", confidences[idx]);
        detections.emplace_back(box, class_names[class_ids[idx]], confidences[idx]);

    }
    return detections;
}
