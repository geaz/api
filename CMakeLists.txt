cmake_minimum_required(VERSION 3.12)
project(syncblinkapi VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 17)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-kissfft.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-rtAudio.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-zLib.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-ixWebsocket.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-SyncBlinkApiArduino.txt)
    
file(GLOB_RECURSE SYNCBLINKDESKTOP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(syncblinkapi SHARED ${SYNCBLINKDESKTOP_SOURCES})
add_dependencies(syncblinkapi
    ext_zLib
    ext_ixws
    ext_rtaudio)
target_link_libraries(syncblinkapi
    kissfft
    zlibstaticd
    ixwebsocket
    rtaudiod)
target_include_directories(syncblinkapi PRIVATE 
    ${syncblinkapiarduino_SOURCE_DIR}/src
    ${syncblinkapiarduino_SOURCE_DIR}/src/audio
    ${syncblinkapiarduino_SOURCE_DIR}/src/network/websocket/messages)

# Necessary for ixWebsocket
if(WIN32)
    target_link_libraries(syncblinkapi ws2_32)
endif()