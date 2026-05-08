add_subdirectory(SDL EXCLUDE_FROM_ALL)

#### Blend2D Library ###
add_subdirectory(blend2d)

#### ImGui ###
add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(imgui PUBLIC
    imgui
    imgui/backends
)
target_link_libraries(imgui PUBLIC SDL3::SDL3)