#ifndef HAND_TRACKING
#define HAND_TRACKING

#include "mediapipe/examples/desktop/controle_module/mediapipe_hands_detector/mediapipe_hands_detector.h"
#include "mediapipe/examples/desktop/controle_module/neural_network/neural_network.h"

#include <vector>

class HandTracking
{
    private:

    // Змінна позначає стан роботи програми
    // Якщо 1 - робота програми запущенна
    // Якщо 0 - робота програми завершенна
    bool isWork;

    // Якщо кисть виявлена, isDetected = 1, інакше - 0
    bool isDetected;

    // Змінна для захоплення відеопотоку (з камери)
    cv::VideoCapture capture;
    // Якщо змінні присвоєно true - процес захоплення відеопотоку працює
    // Якщо змінні присвоєно false - завершити захоплення відеопотоку
    bool grab_frames;
    // Змінна зберігає кадр відеопотоку
    cv::Mat camera_frame_raw;
    // Назва вікна, у якому відображаються оброблені кадри
    std::string windowName = "Image";


    //=====Frame_Processing=====//
    cv::Scalar lower{0,250,250};
    cv::Scalar upper{5,255,255};
    
    cv::Mat frameBlur;
    cv::Mat frameHSV;
    cv::Mat frameMask;

    std::vector <std::vector<cv::Point>> contours;
    std::vector <cv::Vec4i> hierarchy;
    std::vector <std::vector<cv::Point>> contoursPoly;

    int area;
    double perimeter;
    //=====Frame_Processing=====//

    //=====Center_of_mass=====//
    cv::Point point;
    int X, Y;
    int numbPoints;
    //=====Center_of_mass=====//

    //===================================================//

    void calculateCenterOfMass()
    {
        numbPoints = 0;
        X = 0, Y = 0;
        for(int numbOfContours = 0; numbOfContours < contoursPoly.size(); numbOfContours++)
        {
            numbPoints += contoursPoly[numbOfContours].size();
            for(int numbPointsOfContour = 0; numbPointsOfContour < contoursPoly[numbOfContours].size(); numbPointsOfContour++)
            {
                X += contoursPoly[numbOfContours][numbPointsOfContour].x;
                Y += contoursPoly[numbOfContours][numbPointsOfContour].y;
            }

        }

        X = X/(double)numbPoints;
        Y = Y/(double)numbPoints;

        point.x = X; point.y = Y;

        cv::circle(camera_frame_raw, point, 5, cv::Scalar(255,0,255), cv::FILLED);

    }

    void showVideo()
    {
        cv::imshow(windowName, camera_frame_raw);
        cv::waitKey(1);
    }

    void frameProcessing()
    {
        cv::GaussianBlur(camera_frame_raw, frameBlur, cv::Size(3,3), 3.0, 0.0);
        cv::cvtColor(frameBlur, frameHSV, cv::COLOR_BGR2HSV);
        cv::inRange(frameHSV, lower, upper, frameMask);

        // cv::imshow("Image", img);
        //cv::imshow("Image Mask", frameMask);
        //cv::waitKey(1);

        // Знаходимо контури червоного квадрата-рамки
        cv::findContours(frameMask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        //std::cout << "contours     =    " << contours.size() << '\n';
            contoursPoly.resize(contours.size());

            for(int numbOfContours = 0; numbOfContours < contours.size(); numbOfContours++)
            {
                area = cv::contourArea(contours[numbOfContours]);
                if(area > 350)
                {
                    perimeter = cv::arcLength(contours[numbOfContours], true);
                    if(perimeter > 270)
                    {
                        cv::approxPolyDP(contours[numbOfContours], contoursPoly[numbOfContours], perimeter*0.03, true);
                        //cv::drawContours(camera_frame_raw, contoursPoly, numbOfContours, cv::Scalar(0,255,0),2);
                    }

                }
            }
        
    }

    public:

    HandTracking()
    {
        point.x = -1;
        point.y = -1;

        capture.open(0);

        if(capture.isOpened() == 0)
        {
            std::cerr << "HandTracking: Cant open VideoCapture\n";
        }

    }

    ~HandTracking()
    {
        capture.release();

    }

    bool processTracking()
    {
        isWork = true;

        // Створюємо об'єкт класу MediaPipeHandsDetector
        // для розпізнавання кисті руки
        MediaPipeHandsDetector handsDetector(isDetected);

        LOG(INFO) << "Start grabbing and processing frames.";

        grab_frames = true;
        // Розпочинаємо роботу з відеопотоком
        while (grab_frames) 
        {
            // Отримуємо кадр відеопотоку
            capture >> camera_frame_raw;
            cv::flip(camera_frame_raw, camera_frame_raw, /*flipcode=VERTICAL*/ -1);

            // Якщо матриця порожня, отже завершився процес захоплення відео
            // Закінчуємо цикл
            if (camera_frame_raw.empty()) 
            {
                LOG(INFO) << "Empty frame, end of video reached.";
                grab_frames = false;
                break;
            }

            // Запускаємо процес розпізнавання кисті руки у кадрі
            absl::Status run_status = handsDetector.RunMPPGraph(camera_frame_raw);

            if (!run_status.ok()) {
                //LOG(ERROR) << "Failed to run the graph: " << run_status.message();
                isWork = false;
                return false;
            } 
            else 
            {
                //LOG(INFO) << "Success!";
            }

            if(isDetected)
            {
            // Обробка кадру для корегування положення камери
            frameProcessing();
            // Обчислення центру мас кисті
            calculateCenterOfMass();

            //********************************//
            //********************************//
            }
            else
            {
                point.x = -1;
                point.y = -1;
            }

            // Відображення оброблених кадрів
            showVideo();

        }


        return true;
    }

    bool &getIsWork()
    {
        return isWork;
    }

    cv::Point &getPoint()
    {
        return point;
    }

};





#endif // HAND_TRACKING