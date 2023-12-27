#include "hikvisionccd_cv_utils/hikvision/yukihikccd.hpp"
#include <cstddef>
#include <iostream>
#include "hikvisionccd_cv_utils/hikvision/MvCameraControl.h"

namespace ykhik {

YukiHikCCD::YukiHikCCD() {
    INIT_YUKIHIKCCD();
    CHECK_HIK_DEVICES_ACCESS();
}

YukiHikCCD::~YukiHikCCD() {
    CLOSE_CCD_DEVICE();
}

void YukiHikCCD::INIT_YUKIHIKCCD() {
    // Get Hikvision SDK version.
    SDK_VERSION = MV_CC_GetSDKVersion();
    std::cout << "[SUCCESS]Hikversion_SDK_Version:" << SDK_VERSION << std::endl;

    memset(&hik_ccd_devices, 0, sizeof(MV_CC_DEVICE_INFO));
    ret = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &hik_ccd_devices);
    if (ret != MV_OK) {
        std::cout << "[WARING]Not Found Hikversion CCD Devices!!!" << std::endl;
    }
    std::cout << "[SUCCESS]Found Hikversion CCD Devices: " << hik_ccd_devices.nDeviceNum << std::endl;
}

void YukiHikCCD::CHECK_HIK_DEVICES_ACCESS() {
    if (hik_ccd_devices.nDeviceNum > 0) {
        for (unsigned int i = 0; i < hik_ccd_devices.nDeviceNum; i++) {
        std::cout << "[INFO]Hik Device: " << i << std::endl;
        MV_CC_DEVICE_INFO *pDeviceInfo = hik_ccd_devices.pDeviceInfo[i];

        bool access = MV_CC_IsDeviceAccessible(pDeviceInfo, 1);

        if (!access) {
            std::cout << "[ERROR]Not Access Device: " << i << std::endl;
        } else {
            std::cout << "[SUCCESS]Access Device: " << i << std::endl;
        }
        }
    } else {
        std::cout << "[ERROR]Check Devices Not Find Devices!!!" << std::endl;
    }
}

bool YukiHikCCD::OPEN_CCD_DEVICE() {
    if (hik_ccd_devices.nDeviceNum == 0) {
        std::cout << "[ERROR]Open CCD Device Not Found Hikversion Devices!!!" << std::endl;
        return false;
    } else if (SELECTED_CAM_NUM >= hik_ccd_devices.nDeviceNum) {
        std::cout << "[ERROR]Device index out range!!!" << std::endl;
        return false;
    }

    // Create CCD Handle.
    ret = MV_CC_CreateHandle(&CCD_HANDLE,
                            hik_ccd_devices.pDeviceInfo[SELECTED_CAM_NUM]);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Create CCD handle failed!!!" << std::endl;
        return false;
    }

    // Open CCD Device.
    ret = MV_CC_OpenDevice(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Open CCD Device failed!!!" << std::endl;
        return false;
    } else {
        std::cout << "[SUCCESS]Device Open. Device: " << SELECTED_CAM_NUM
                << std::endl;
    }

    // Set trigger mod off.
    ret = MV_CC_SetEnumValue(CCD_HANDLE, "TriggerMode", 0);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Set TriggerMod off failed!!!" << std::endl;
        return false;
    }

    REVEIVE_HIK_OPTIONS();
    RECEIVE_HIK_FRAME();

    return true;
}

void YukiHikCCD::REVEIVE_HIK_OPTIONS() {
    ret = MV_CC_GetIntValue(CCD_HANDLE, "Width", &hik_param);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Get Width failed!!!" << std::endl;
    }
    IMG_WIDTH = hik_param.nCurValue;

    ret = MV_CC_GetIntValue(CCD_HANDLE, "Height", &hik_param);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Get Height failed!!!" << std::endl;
    }
    IMG_HEIGHT = hik_param.nCurValue;
    std::cout << "[INFO]IMG SIZE: " << std::to_string(IMG_WIDTH) << "x"
                << std::to_string(IMG_HEIGHT) << std::endl;

    MVCC_FLOATVALUE stFloatVal;
    ret = MV_CC_GetFloatValue(CCD_HANDLE, "AcquisitionFrameRate", &stFloatVal);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Get FPS failed!!!" << std::endl;
    }
    FPS = stFloatVal.fCurValue;
    std::cout << "[INFO]FPS: " << std::to_string(FPS) << std::endl;
};

void YukiHikCCD::RECEIVE_HIK_FRAME() {
    memset(&hik_param, 0, sizeof(MVCC_INTVALUE));
    ret = MV_CC_GetIntValue(CCD_HANDLE, "PayloadSize", &hik_param);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Get CCD Data packets failed!!!" << std::endl;
    }
    payload_size = hik_param.nCurValue;
    std::cout << "[INFO]CCD PayloadSize: " << payload_size << std::endl;

    ret = MV_CC_StartGrabbing(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Grab image failed!!!" << std::endl;
    }
    memset(&hik_frame, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    data = (unsigned char *)malloc(sizeof(unsigned char) * (payload_size));
}

void YukiHikCCD::HKCCD_TO_CV() {
    ret = MV_CC_GetOneFrameTimeout(CCD_HANDLE, data, payload_size, &hik_frame,
                                    1000);
    if (ret != MV_OK) {
        free(data);
        data = NULL;
    }

    if (data != NULL) {
        if (hik_frame.enPixelType == PixelType_Gvsp_Mono8) {
            CV_MAT = cv::Mat(hik_frame.nHeight, hik_frame.nWidth, CV_8UC1, data);
        } else if (hik_frame.enPixelType == PixelType_Gvsp_BayerRG8) {
            CV_MAT = cv::Mat(hik_frame.nHeight, hik_frame.nWidth, CV_8UC1, data);
            if (!CV_MAT.empty()) cv::cvtColor(CV_MAT, CV_MAT, cv::COLOR_BayerBG2BGR);
        } else if (hik_frame.enPixelType == PixelType_Gvsp_RGB8_Packed) {
            CV_MAT = cv::Mat(hik_frame.nHeight, hik_frame.nWidth, CV_8UC3, data);
        } 
    }
}

void YukiHikCCD::CLOSE_CCD_DEVICE() {
    ret = MV_CC_StopGrabbing(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Stop Grabbing failed!!!" << std::endl;
    }

    ret = MV_CC_CloseDevice(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Destory CCD handle failed!!!" << std::endl;
    }

    ret = MV_CC_DestroyHandle(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Close CCD handle failed!!!" << std::endl;
    }
};

}