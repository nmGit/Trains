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
add_executable(Trains 
	"source/main.cpp"
)


##### External Libraries #####
add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(source)

##### Link Libraries #####
target_link_libraries(Trains PRIVATE Drafter)
target_link_libraries(Trains PRIVATE CityPlanner)