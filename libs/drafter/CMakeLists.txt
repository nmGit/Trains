cmake_minimum_required(VERSION 3.20)
project(drafter)
enable_language(CXX)

add_library(Drafter)

target_sources(Drafter PRIVATE 
    source/Canvas.cpp
    source/Shape.cpp
    source/Shapes/Circle.cpp
 "source/Shapes/Label.cpp" "include/Drafter/Shapes/Label.h")


target_include_directories(Drafter PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

##### Log Library #####
target_link_libraries(Drafter PUBLIC Log)

##### Blend2D Library #####
target_link_libraries(Drafter PUBLIC blend2d::blend2d)

##### SDL3 Library #####
target_link_libraries(Drafter PUBLIC SDL3::SDL3)

##### SDL3_ttf Library #####
target_link_libraries(Drafter PUBLIC SDL3_ttf::SDL3_ttf)

##### Events Library #####
target_link_libraries(Drafter PUBLIC Events)
