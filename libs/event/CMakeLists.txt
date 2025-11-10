cmake_minimum_required(VERSION 3.20)
project(events)
enable_language(CXX)

add_library(Events INTERFACE)

target_include_directories(Events INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)