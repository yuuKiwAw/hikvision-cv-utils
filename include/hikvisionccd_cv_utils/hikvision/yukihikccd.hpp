/**************************************************
Copyright(C), 2022, YukiSoftware. All rights reserved.
Author: Yuki(ZhangTuo)
Version: 1.0
**************************************************/
#pragma once

#include "MvCameraControl.h"
#include <iostream>
#include <opencv2/opencv.hpp>

namespace ykhik {

class YukiHikCCD {
private:
    int SDK_VERSION = 0; // Hikversion SDK version.
    int ret = MV_OK;
    unsigned int payload_size;
    MV_CC_DEVICE_INFO_LIST
    hik_ccd_devices; // Current device's Hikversion's devices list.
    MVCC_INTVALUE hik_param;
    MV_FRAME_OUT_INFO_EX hik_frame; // Hikversion ccd image.
    void *CCD_HANDLE = nullptr;     // Current selected ccd camera handler.
    unsigned char *data;
    void INIT_YUKIHIKCCD();
    void CHECK_HIK_DEVICES_ACCESS(); // Check Hikversion Devices available.
    void REVEIVE_HIK_OPTIONS();      // Receive Hikversion MVS options value.
    void RECEIVE_HIK_FRAME();        // Receive stream from Hikversion ccd.

public:
    unsigned int SELECTED_CAM_NUM = 0;          // Selected ccd camera number from hik_ccd_devices_list.
    cv::Mat CV_MAT; // Hikversion ccd to opencv mat var.
    int IMG_WIDTH;
    int IMG_HEIGHT;
    float FPS;
    YukiHikCCD();
    ~YukiHikCCD();
    bool OPEN_CCD_DEVICE();
    void CLOSE_CCD_DEVICE();
    void HKCCD_TO_CV(); // Format CCD image data to opencv mat data.

    bool ExposureAutoMod(float value);
    bool ExposureManualMod(float value);
    bool Brightness(float value);
};

} // namespace CCD