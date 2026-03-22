cmake_minimum_required(VERSION 3.20)
project(drafter)
enable_language(CXX)

add_library(Drafter)

target_sources(Drafter PRIVATE 
    source/Canvas.cpp
    source/Shape.cpp
    source/Shapes/Circle.cpp
    source/Shapes/Label.cpp
    source/Shapes/NGon.cpp
    source/Shapes/HexGrid.cpp
    include/Drafter/Shapes/Label.h
    include/Drafter/Shapes/NGon.h
    include/Drafter/Shapes/HexGrid.h 
    include/Drafter/Utils.h
    source/Utils.cpp
)

target_include_directories(Drafter PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(Drafter PUBLIC Log)
target_link_libraries(Drafter PUBLIC blend2d::blend2d)
target_link_libraries(Drafter PUBLIC SDL3::SDL3)
target_link_libraries(Drafter PUBLIC Events)
