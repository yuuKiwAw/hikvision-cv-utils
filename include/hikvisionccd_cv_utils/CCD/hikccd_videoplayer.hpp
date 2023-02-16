#pragma once

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <chrono>
#include "hikvisionccd_cv_utils/hikvision/yukihikccd.hpp"

namespace CCD {

class HikCCDVideoPlayer {
private:
    int input_fps = 0;
    int output_fps = 0;
    int VIDEO_WIDTH = 0;
    int VIDEO_HEIGHT = 0;
    cv::Mat frame;
    cv::Size output_size;

    bool stop_flag = true;
    std::mutex player_thread_mutex;
    std::thread player_thread;

    ykhik::YukiHikCCD yuki_HIKCCD;

    void run();

public:
    void start(int ccd_index, int fps = 30);
    void stop();
    cv::Mat next();

    int get_FPS();
    bool get_STOP_FLAG();
};

}