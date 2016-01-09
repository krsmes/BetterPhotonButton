cmake_minimum_required(VERSION 3.3)
project(BetterPhotonButton)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
        firmware
        firmware/examples
        ../../firmware/system/inc
        ../../firmware/hal/inc
        ../../firmware/hal/shared
        ../../firmware/hal/src/stm32f2xx
        ../../firmware/services/inc
        ../../firmware/wiring/inc
        ../../firmware/user/inc
)

set(SOURCE_FILES
        BetterPhotonButton.cpp BetterPhotonButton.h
        Animations.cpp
        SerialTesting.cpp)

#add_executable(BetterPhotonButton ${SOURCE_FILES})