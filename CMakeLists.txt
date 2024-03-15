project(AR_Recognition)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(calib src/calibration.cpp src/function.cpp include/function.h)
add_executable(ar src/augmented_reality.cpp src/function.cpp include/function.h)
target_link_libraries(calib ${OpenCV_LIBS})
target_link_libraries(ar ${OpenCV_LIBS})