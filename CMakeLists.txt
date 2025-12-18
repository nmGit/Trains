cmake_minimum_required(VERSION 3.28)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)



##### CMake Output Settings #####
# From https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-cmake.md
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")
set(SDL_SHARED ON)

##### Main Executable #####
project(Trains)

if(BUILD_GAME)
	set(EXECUTABLE_NAME TrainGame)
	add_executable(${EXECUTABLE_NAME} 
		"source/main.cpp"
	)
endif()

##### Build Tests #####
if(TRAINS_BUILD_TESTS)
	
endif()

##### Build Examples #####
if(TRAINS_BUILD_EXAMPLES)
	set(EXECUTABLE_NAME TrianGameExamples)
	add_executable(${EXECUTABLE_NAME} 
		"examples/Examples.cpp"
	)
	message(STATUS "Building Examples")
	add_subdirectory(examples)
endif()

##### External Libraries #####
add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(source)

##### Link Libraries #####
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Drafter)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE CityPlanner)
