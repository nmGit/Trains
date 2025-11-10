cmake_minimum_required(VERSION 3.20)
project(cityplanner)
enable_language(CXX)

add_library(CityPlanner)

target_sources(CityPlanner PRIVATE 
	source/City.cpp
	source/World.cpp
	source/Region.cpp
)

target_include_directories(CityPlanner PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

##### Events Library #####
target_link_libraries(CityPlanner PUBLIC Events)


##### Log Library #####
target_link_libraries(CityPlanner PUBLIC Log)
