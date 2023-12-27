cmake_minimum_required(VERSION 3.24)
project(hikvision-cv-utils VERSION 0.1.0)

set(PackageName "hikvision_cv_utils")
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

set(hikvision_cv_utils_lib_path ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(INCLUDE_INSTALL_FILES
    ./include/hikvisionccd_cv_utils/hikvision/CameraParams.h
    ./include/hikvisionccd_cv_utils/hikvision/MvCameraControl.h
    ./include/hikvisionccd_cv_utils/hikvision/MvErrorDefine.h
    ./include/hikvisionccd_cv_utils/hikvision/MvISPErrorDefine.h
    ./include/hikvisionccd_cv_utils/hikvision/MvObsoleteInterfaces.h
    ./include/hikvisionccd_cv_utils/hikvision/MvSdkExport.h
    ./include/hikvisionccd_cv_utils/hikvision/ObsoleteCamParams.h
    ./include/hikvisionccd_cv_utils/hikvision/PixelType.h
    ./include/hikvisionccd_cv_utils/CCD/hikccd_videoplayer.hpp
)
set(SRC_LIST
    ./src/hikvisionccd_cv_utils/hikvision/yukihikccd.cpp
    ./src/hikvisionccd_cv_utils/CCD/hikccd_videoplayer.cpp
)
set(LIBRARY_OUTPUT_PATH ${hikvision_cv_utils_lib_path})
set(export_runtime_path)
set(export_library_path)
set(export_archive_path)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE"Release" CACHE STRING"" FORCE)
    endif()
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(OpenCV 4.5.5 REQUIRED)

if(WIN32)
    message(STATUS "WIN32 EXPORT ALL SYMBOLS")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    link_directories(./lib/Win64)
ENDIF()

add_library(${PackageName}
    SHARED
        ${SRC_LIST}
)
target_link_libraries(${PackageName} PRIVATE ${OpenCV_LIBS} MvCameraControl)

add_subdirectory(examples)

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(export_runtime_path bin)
    set(export_library_path lib)
    set(export_archive_path lib)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(export_runtime_path debug/bin)
    set(export_library_path debug/lib)
    set(export_archive_path debug/lib)
endif()

install(TARGETS ${PackageName}
    EXPORT ${PackageName}
    RUNTIME DESTINATION ${export_runtime_path}
    LIBRARY DESTINATION ${export_library_path}
    ARCHIVE DESTINATION ${export_archive_path}
)

install(DIRECTORY include/ DESTINATION include)
install(EXPORT ${PackageName}
    FILE hikvision_cv_utilsConfig.cmake
    NAMESPACE hik::
    DESTINATION share/${PackageName}
)
