cmake_minimum_required(VERSION 3.12)
project(glowflyapi VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 17)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-kissfft.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-rtAudio.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-zLib.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-ixWebsocket.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-GlowFlyApiArduino.txt)
    
file(GLOB_RECURSE GLOWFLYDESKTOP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(glowflyapi SHARED ${GLOWFLYDESKTOP_SOURCES})
add_dependencies(glowflyapi
    ext_zLib
    ext_ixws
    ext_rtaudio)
target_link_libraries(glowflyapi
    kissfft
    zlibstatic
    ixwebsocket
    rtaudio)
target_include_directories(glowflyapi PRIVATE 
    ${glowflyapiarduino_SOURCE_DIR}/src
    ${glowflyapiarduino_SOURCE_DIR}/src/visualization
    ${glowflyapiarduino_SOURCE_DIR}/src/network/websocket/commands)

# Necessary for ixWebsocket
if(WIN32)
    target_link_libraries(glowflyapi ws2_32)
endif()