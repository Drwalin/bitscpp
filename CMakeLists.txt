cmake_minimum_required(VERSION 3.8)
project(bitscpp)
cmake_policy(SET CMP0115 OLD)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BITSCPP_BUILD_TEST "Build bitscpp tests" ON)
option(BITSCPP_ENABLE_LTO "Bitscpp enable LTO" ON)
option(BITSCPP_INLINE_V2 "Bitscpp enable inline V2" ON)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	option(BITSCPP_INLINE_V2 "Bitscpp enable inline V2" OFF)
endif()

if(BITSCPP_INLINE_V2)
	add_compile_definitions(BITSCPP_ENABLE_INLINE_V2=true)
endif()

if(BITSCPP_ENABLE_LTO)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
endif()

include_directories(./include/)

aux_source_directory(./src/ source_files)
add_library(bitscpp STATIC
	${source_files}
	./thirdparty/half_float/HalfFloat.cpp
)

if(BITSCPP_BUILD_TEST)
	aux_source_directory(./include/bitscpp header_files)
	aux_source_directory(./tests test_source_files)
	
	add_executable(test_bitscpp
		${header_files}
		${test_source_files}
	)
	target_link_libraries(test_bitscpp bitscpp)
endif()
