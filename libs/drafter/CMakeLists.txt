cmake_minimum_required(VERSION 3.20)
project(drafter)
enable_language(CXX)

add_library(Drafter)

target_sources(Drafter PRIVATE
    source/Camera.cpp
    source/Canvas.cpp
    source/Shape.cpp
    source/Shapes/Circle.cpp
    source/Shapes/Label.cpp
    source/Shapes/NGon.cpp
    source/Shapes/HexGrid.cpp
    source/Overlay/Overlay.cpp
    source/Overlay/ImGuiOverlay.cpp
    source/Overlay/DebugOverlay.cpp
    source/Overlay/DataTreeOverlay.cpp
    include/Drafter/Shapes/Label.h
    include/Drafter/Shapes/NGon.h
    include/Drafter/Camera.h
    include/Drafter/Shapes/HexGrid.h
    include/Drafter/Utils.h
    source/Utils.cpp
    include/Drafter/Utils/Color.h
    source/Utils/Color.cpp
    include/Drafter/Utils/Font.h
    source/Utils/Font.cpp
    include/Drafter/Overlay/Overlay.h
    include/Drafter/Overlay/ImGuiOverlay.h
    include/Drafter/Overlay/DebugOverlay.h
    include/Drafter/Overlay/DataTreeOverlay.h
)

target_include_directories(Drafter PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(Drafter PUBLIC Log)
target_link_libraries(Drafter PUBLIC blend2d::blend2d)
target_link_libraries(Drafter PUBLIC SDL3::SDL3)
target_link_libraries(Drafter PUBLIC Events)
target_link_libraries(Drafter PUBLIC imgui)
