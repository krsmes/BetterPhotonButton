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
        firmware/BetterPhotonButton.cpp firmware/BetterPhotonButton.h
        firmware/examples/AnimateAccel.cpp
        firmware/examples/Animations.cpp
        firmware/examples/SerialTesting.cpp
        firmware/examples/SimpleRainbow.cpp
        firmware/examples/Incrementor.cpp
        firmware/examples/ToneTest.cpp firmware/examples/PlaySongs.cpp firmware/examples/Whites.cpp)

add_executable(BetterPhotonButton ${SOURCE_FILES})