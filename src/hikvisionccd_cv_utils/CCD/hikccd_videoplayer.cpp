#include "hikvisionccd_cv_utils/CCD/hikccd_videoplayer.hpp"
#include "hikvisionccd_cv_utils/hikvision/yukihikccd.hpp"

namespace CCD {

void HikCCDVideoPlayer::start(int ccd_index, int fps) {
    stop_flag = false;

    yuki_HIKCCD.SELECTED_CAM_NUM = ccd_index;
    if (!yuki_HIKCCD.OPEN_CCD_DEVICE()) {
        return;
    }
    VIDEO_HEIGHT = yuki_HIKCCD.IMG_HEIGHT;
    VIDEO_WIDTH = yuki_HIKCCD.IMG_WIDTH;

    input_fps = (int)yuki_HIKCCD.FPS;
    if (input_fps <= 0)
        input_fps = 60;
    if (fps == 0) {
        output_fps = input_fps;
    } else {
        output_fps = fps;
    }

    player_thread = std::thread(&HikCCDVideoPlayer::run, this);
    player_thread.detach();
}

void HikCCDVideoPlayer::stop() {
    if (!stop_flag) {
        stop_flag = true;
        yuki_HIKCCD.CLOSE_CCD_DEVICE();
    }
}

void HikCCDVideoPlayer::run() {
    while (!stop_flag) {
        player_thread_mutex.lock();
        yuki_HIKCCD.HKCCD_TO_CV();
        frame = yuki_HIKCCD.CV_MAT;
        player_thread_mutex.unlock();

        cv::waitKey(1);
    }
}

cv::Mat HikCCDVideoPlayer::next() {
    player_thread_mutex.lock();
    cv::Mat m_frame;
    if (!frame.empty()) {
        m_frame = frame.clone();
    }
    player_thread_mutex.unlock();

    return m_frame;
}

int HikCCDVideoPlayer::get_FPS() {
    return output_fps;
}

bool HikCCDVideoPlayer::get_STOP_FLAG() {
    return stop_flag;
}

ykhik::YukiHikCCD *HikCCDVideoPlayer::YukiHikCCD_Handle() {
    return &yuki_HIKCCD;
}

}