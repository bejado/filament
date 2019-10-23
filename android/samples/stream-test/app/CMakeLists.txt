cmake_minimum_required(VERSION 3.4.1)

find_library(log-lib log)
find_library(android-lib android)
find_library(nativewindow-lib nativewindow)

add_library(native-lib SHARED src/main/cpp/native-lib.cpp)

target_link_libraries(native-lib ${log-lib} ${android-lib} ${nativewindow-lib} EGL)
