#ifndef MEDIAPIPE_HANDS_DETECTOR
#define MEDIAPIPE_HANDS_DETECTOR

#include <iostream>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/logging.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_imgcodecs_inc.h"



constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char kHandCountOutputStream[] = "hand_count";
constexpr char kLandmarksOutputStream[] = "landmarks";
constexpr char kMultiPalmRects[] = "multi_palm_rects";


class MediaPipeHandsDetector
{
private:
    mediapipe::CalculatorGraph graph;

    std::unique_ptr<mediapipe::OutputStreamPoller> outputVideoPoller;
    std::unique_ptr<mediapipe::OutputStreamPoller> outputLandmarksPoller;
    std::unique_ptr<mediapipe::OutputStreamPoller> outputHandCountPoller;

    ::mediapipe::Packet handCountPacket;
    ::mediapipe::Packet landmarksPacket;
    ::mediapipe::Packet imagePacket;

    // Змінна зберігає шлях до конфігураційного файлу (*.pbtxt)
    std::string calculator_graph_config_contents;
    // Змінна зберігає конфігураційні дані
    mediapipe::CalculatorGraphConfig config;
    // Часова мітка
    size_t frame_timestamp_us;

protected:
    absl::Status Setup(std::string calculator_graph_config_file);

    absl::Status Shutdown();

public:
    MediaPipeHandsDetector(std::string calculator_graph_config_file = "mediapipe/graphs/hand_tracking/custom_hand_tracking_desktop_live.pbtxt");

    virtual ~MediaPipeHandsDetector();
    
    absl::Status RunMPPGraph(cv::Mat &camera_frame_raw);
};

#endif // MEDIAPIPE_HANDS_DETECTOR