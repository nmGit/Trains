cmake_minimum_required(VERSION 3.20)
project(log)
enable_language(CXX)

add_library(Log)

target_sources(Log PRIVATE 
	"source/Log.cpp"
)

target_include_directories(Log PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
