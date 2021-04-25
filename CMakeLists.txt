cmake_minimum_required (VERSION 3.5)

project(Aurora)

# C++ 11 is required
set(CMAKE_CXX_STANDARD 11)

OPTION(AURORA_DOUBLE_AS_FLOAT "Use 64-bit floats" OFF)

IF (AURORA_DOUBLE_AS_FLOAT)
  ADD_DEFINITIONS ( -D AURORA_DOUBLE_AS_FLOAT )
ENDIF()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(src/core/)
include_directories(src/math/)
include_directories(${PROJECT_SOURCE_DIR}/external/include)

link_directories(${PROJECT_SOURCE_DIR}/external/lib)

file(GLOB_RECURSE SRCS ./src/core/*.cpp ./src/math/*.cpp ./src/*.cpp)
file(GLOB_RECURSE HEADERS ./src/core/*.h ./src/math/*.h)
source_group("Header Files" FILES ${HEADERS})

###########################################################################
# glog

SET(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
IF(WIN32)
  ADD_DEFINITIONS( -D GOOGLE_GLOG_DLL_DECL= )
ENDIF()
ADD_SUBDIRECTORY(src/ext/glog)
SET_PROPERTY(TARGET glog logging_unittest demangle_unittest utilities_unittest stl_logging_unittest signalhandler_unittest stacktrace_unittest symbolize_unittest PROPERTY FOLDER "ext")
INCLUDE_DIRECTORIES (
  src/ext/glog/src
  ${CMAKE_BINARY_DIR}/src/ext/glog
)

add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

target_link_libraries( ${PROJECT_NAME} 
    PUBLIC 
	tbb
	glog::glog
)