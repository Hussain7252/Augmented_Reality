project(AR_Recognition)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(main src/main.cpp src/function.cpp include/function.h)

target_link_libraries(main ${OpenCV_LIBS})