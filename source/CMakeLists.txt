target_sources(${EXECUTABLE_NAME} PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/ViewModel.cpp
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}
)

if(${BUILD_GAME})
target_sources(${EXECUTABLE_NAME} PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/Game.cpp
)
endif()

##### Interface #####
add_subdirectory(interface)

##### Engine #####
add_subdirectory(engine)
