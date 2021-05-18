#include "mediapipe_hands_detector.h"

    absl::Status MediaPipeHandsDetector::Setup(std::string calculator_graph_config_file)
    {
        MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
            calculator_graph_config_file,
            &calculator_graph_config_contents));

        LOG(INFO) << "Get calculator graph config contents: " << calculator_graph_config_contents;
        
        config = mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
                calculator_graph_config_contents);

        LOG(INFO) << "Initialize the calculator graph.";
        MP_RETURN_IF_ERROR(graph.Initialize(config));

        //===========================================================================//
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller, graph.AddOutputStreamPoller(kLandmarksOutputStream));
        outputLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(poller));

        ASSIGN_OR_RETURN(poller, graph.AddOutputStreamPoller(kHandCountOutputStream));
        outputHandCountPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(poller));

        ASSIGN_OR_RETURN(poller, graph.AddOutputStreamPoller(kOutputStream));
        outputVideoPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(poller));

       //===========================================================================//

        LOG(INFO) << "Start running the calculator graph.";
        MP_RETURN_IF_ERROR(graph.StartRun({}));

        return absl::Status();
    }

    absl::Status MediaPipeHandsDetector::Shutdown()
    {
        LOG(INFO) << "Shutting down.";
        MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
        graph.WaitUntilDone();
    }

    MediaPipeHandsDetector::MediaPipeHandsDetector(std::string calculator_graph_config_file)                   
    {
        LOG(INFO) << Setup(calculator_graph_config_file);
    }

    MediaPipeHandsDetector::~MediaPipeHandsDetector()
    {
        Shutdown();
    }

    absl::Status MediaPipeHandsDetector::RunMPPGraph(cv::Mat &camera_frame_raw)
    {
        LOG(INFO) << "Start grabbing and processing frames.";

        cv::Mat camera_frame;
        cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
        cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);

        // Обгортаємо Mat в ImageFrame (формуємо пакет із зображенням)
        auto input_frame = absl::make_unique<::mediapipe::ImageFrame>(
            ::mediapipe::ImageFormat::SRGB, camera_frame.cols, camera_frame.rows,
            ::mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        cv::Mat input_frame_mat = ::mediapipe::formats::MatView(input_frame.get());
        camera_frame.copyTo(input_frame_mat);

        // Формуємо часому мітку
        size_t frame_timestamp_us = (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
        // Передаємо у граф пакет із зображенням
        MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
            kInputStream, ::mediapipe::Adopt(input_frame.release())
            .At(::mediapipe::Timestamp(frame_timestamp_us))));

        // Отримуємо пакет із графа (кількість виявлених кистей руки)
        if (!outputHandCountPoller->Next(&handCountPacket))
        {
            absl::string_view msg("Error when outputHandCountPoller try to get the result pack from graph!");
        }
        // Отримуємо дані з пакету
        auto &hand_count = handCountPacket.Get<int>();

        if (hand_count != 0)
        {
            LOG(INFO) << "Found hand count : " << hand_count;

            // Отримуємо пакет із графа (Hand landmsrks)
            if (!outputLandmarksPoller->Next(&landmarksPacket))
            {
                absl::string_view msg("Error when outputLandmarksPoller try to get the result pack from graph!");
            }
            // Отримуємо дані з пакету
            auto &multi_hand_landmarks = landmarksPacket.Get<std::vector<::mediapipe::NormalizedLandmarkList>>();

            //================================
            LOG(INFO) << "#Multi Hand landmarks: " << multi_hand_landmarks.size();
            int hand_id = 0;
            for (const auto &single_hand_landmarks : multi_hand_landmarks)
            {
                ++hand_id;
                LOG(INFO) << "Hand [" << hand_id << "]:";
                // for (int i = 0; i < single_hand_landmarks.landmark_size(); ++i)
                // {
                //     const auto &landmark = single_hand_landmarks.landmark(i);
                //     LOG(INFO) << "\tLandmark [" << i << "]: ("
                //               << landmark.x() << ", "
                //               << landmark.y() << ", "
                //               << landmark.z() << ")";
                              
                              
                // }
            }
            //================================
        }

        // Get the graph result packet, or stop if that fails.
        if (!outputVideoPoller->Next(&imagePacket))
        {
            absl::string_view msg("Error when outputVideoPoller try to get the result pack from graph!");
            return absl::Status(absl::StatusCode::kUnknown, msg);
        }
        auto &output_frame_mat_view = imagePacket.Get<::mediapipe::ImageFrame>();

        // Convert back to opencv for display or saving.
        cv::Mat output_frame_mat = ::mediapipe::formats::MatView(&output_frame_mat_view);
        cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);

        // Присвоюємо кадру, який передали у функцію, оброблений кадр
        output_frame_mat.copyTo(camera_frame_raw);

        return absl::Status();
    }
