cmake_minimum_required(VERSION 3.8)
project(bitscpp)
cmake_policy(SET CMP0115 OLD)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BITSCPP_BUILD_TEST "Build bitscpp tests" ON)

include_directories(./include/)

aux_source_directory(./src/ source_files)
add_library(bitscpp STATIC
	${source_files}
)

if(BITSCPP_BUILD_TEST)
	aux_source_directory(./include/bitscpp header_files)
	aux_source_directory(./tests source_files)
	
	add_executable(test
		${header_files}
		${source_files}
		./thirdpart/half_float/HalfFloat.cpp
	)
endif()
