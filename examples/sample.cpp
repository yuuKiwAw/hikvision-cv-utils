#include "hikvisionccd_cv_utils/CCD/hikccd_videoplayer.hpp"
#include <mutex>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
  CCD::HikCCDVideoPlayer hik_CCD_video_player;
  hik_CCD_video_player.start(0, 65);

  bool quit = false;

  std::mutex cv_mtx;

  while (!hik_CCD_video_player.get_STOP_FLAG()) {
    if (quit) {
      break;
    }

    cv_mtx.lock();
    cv::Mat img = hik_CCD_video_player.next();
    if (!img.empty()) {
      cv::imshow("Hikccd", img);
    }
    cv_mtx.unlock();

    if (27 == cv::waitKey(1))
      quit = true;
  }

  return 0;
}