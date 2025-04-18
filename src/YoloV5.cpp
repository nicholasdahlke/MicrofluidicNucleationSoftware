//
// Created by nicholas on 25.02.25.
//

#include <MicrofluidicNucleation/YoloV5.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>


mfn::YoloV5::YoloV5(const mfn::AnalysisConfig &config)
{
    open(config);
}

void mfn::YoloV5::open(const mfn::AnalysisConfig &config)
{
    spdlog::get("mfn_logger")->info("Reading net file");
    if(!std::filesystem::exists(config.net_file))
        spdlog::get("mfn_logger")->error("An error occurred opening the neural net file.");
    else
        yolo = cv::dnn::readNet(config.net_file.string());

    net_input_size = cv::Size(640.0, 640.0);
    spdlog::get("mfn_logger")->info("Reading net file finished");
    YoloV5::config = config;
    //yolo.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    //yolo.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

}

/*std::vector<mfn::Detection> mfn::YoloV5::process(const cv::Mat &input)
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

    return getDetectionFromOutput(input, (float*)outputs[0].data);
}*/

std::vector<std::vector<mfn::Detection>> mfn::YoloV5::process(const std::vector<cv::Mat> & inputs)
{
    /*std::vector<std::vector<mfn::Detection>> result;
    for(const cv::Mat & input : inputs)
        result.push_back(process(input));
    return result;
*/
#ifdef REINITIALIZE_NET_ALWAYS
    yolo = cv::dnn::readNet(config.net_file.string());
    yolo.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    yolo.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
#endif
    if(yolo.empty())
        spdlog::get("mfn_logger")->error("Error during inference. Net empty");
    spdlog::get("mfn_logger")->info("Generating blob with {} images", inputs.size());

    std::vector<cv::Mat> loc_inputs = inputs;

    if (inputs.size() > config.parallel)
        spdlog::get("mfn_logger")->error("Too many images for batch inference, reduce the config.parallel parameter", loc_inputs.size());


    if (inputs.size() < config.parallel)
    {
        for(int i = 0; i < config.parallel - inputs.size(); i++)
        {
            loc_inputs.emplace_back(640, 640, CV_8UC3, cv::Scalar(0, 0, 0));
        }
    }

    const cv::Mat blob = cv::dnn::blobFromImages(loc_inputs,
                                            1.0/255.0,
                                            net_input_size,
                                            cv::Scalar(0, 0, 0),
                                            true,
                                            false);
    spdlog::get("mfn_logger")->info("Starting inference");
    const std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::vector<cv::Mat> outputs;

    yolo.setInput(blob);
    yolo.forward(outputs, yolo.getUnconnectedOutLayersNames());
#ifdef REINITIALIZE_NET_ALWAYS
    yolo = cv::dnn::Net();
#endif

    const std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    spdlog::get("mfn_logger")->info("Inference finished in {}ms, frame average is {}ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() / loc_inputs.size());
    return getDetectionsFromOutput(inputs, outputs);

}

std::vector<std::vector<mfn::Detection> > mfn::YoloV5::getDetectionsFromOutput(const std::vector<cv::Mat> &inputs, const std::vector<cv::Mat> &outputs) const
{
    std::vector<std::vector<mfn::Detection>> detections;
    constexpr int im_out_size = 25200 * 7;
    auto data = reinterpret_cast<float *>(outputs[0].data);
    for (const auto & input : inputs)
    {
        detections.push_back(getDetectionFromOutput(input, data));
        data += im_out_size;
    }
    return detections;
}


std::vector<mfn::Detection> mfn::YoloV5::getDetectionFromOutput(const cv::Mat &input, float * data) const
{
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    std::vector<std::string> class_names;
    class_names.emplace_back("droplets");
    class_names.emplace_back("droplets_frozen");

    float x_factor = static_cast<float>(input.cols) / net_input_size.width;
    float y_factor = static_cast<float>(input.rows) / net_input_size.height;
    const int rows = 25200;
    const int dimensions = 7; // Always adjust for multi class models

    for (size_t k = 0; k < rows; ++k)
    {
        if(float confidence = data[4]; confidence >= config.confidence_threshold)
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
        detections.emplace_back(boxes[idx], class_names[class_ids[idx]], confidences[idx]);
    }
    return detections;
}

