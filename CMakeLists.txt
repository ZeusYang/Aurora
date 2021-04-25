cmake_minimum_required (VERSION 3.5)

project(Aurora)

# C++ 11 is required
set(CMAKE_CXX_STANDARD 11)

include_directories(src/core/)
include_directories(src/math/)
include_directories(${PROJECT_SOURCE_DIR}/external/include)

link_directories(${PROJECT_SOURCE_DIR}/external/lib)


file(GLOB_RECURSE SRCS ./src/core/*.cpp ./src/math/*.cpp ./src/*.cpp)
file(GLOB_RECURSE HEADERS ./src/core/*.h ./src/math/*.h)
source_group("Header Files" FILES ${HEADERS})

add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

target_link_libraries( ${PROJECT_NAME} 
    PUBLIC 
	tbb
	tbb12
)