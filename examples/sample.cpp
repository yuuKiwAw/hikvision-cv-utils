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

    int key_code =  cv::waitKey(1);
    if (key_code == 27)
      quit = true;
    if (key_code == 'q') {
      hik_CCD_video_player.YukiHikCCD_Handle()->ExposureManualMod(10000);
    }
    if (key_code == 'w') {
      hik_CCD_video_player.YukiHikCCD_Handle()->ExposureAutoMod(1);
    }
    if (key_code == 'e') {
      hik_CCD_video_player.YukiHikCCD_Handle()->ExposureAutoMod(2);
    }
    if (key_code == 'r') {
      hik_CCD_video_player.YukiHikCCD_Handle()->ExposureAutoMod(0);
    }
    if (key_code == 'j') {
      hik_CCD_video_player.YukiHikCCD_Handle()->Brightness(255);
    }
    if (key_code == 'k') {
      hik_CCD_video_player.YukiHikCCD_Handle()->Brightness(0);
    }

  }

  return 0;
}