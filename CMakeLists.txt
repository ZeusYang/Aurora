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

###########################################################################
# header-only libraries

INCLUDE_DIRECTORIES (
  src/ext/header_only
)

###########################################################################
# glog

SET(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
IF(WIN32)
  ADD_DEFINITIONS( -D GOOGLE_GLOG_DLL_DECL= )
ENDIF()
ADD_DEFINITIONS( -D HAVE_SNPRINTF)
ADD_SUBDIRECTORY(src/ext/glog)
SET_PROPERTY(TARGET glog logging_unittest demangle_unittest utilities_unittest stl_logging_unittest signalhandler_unittest stacktrace_unittest symbolize_unittest PROPERTY FOLDER "ext")
INCLUDE_DIRECTORIES (
  src/ext/glog/src
  ${CMAKE_BINARY_DIR}/src/ext/glog
)

###########################################################################
# assimp
ADD_SUBDIRECTORY(src/ext/assimp)
SET_PROPERTY(TARGET assimp assimp_cmd uninstall unit UpdateAssimpLibsDebugSymbolsAndDLLs zlibstatic PROPERTY FOLDER "ext")
INCLUDE_DIRECTORIES (
  src/ext/assimp/include
  ${CMAKE_BINARY_DIR}/src/ext/assimp
)

file(GLOB SRCS ./src/core/*.cpp ./src/*.cpp)
file(GLOB HEADERS ./src/core/*.h )
source_group("Header Files" FILES ${HEADERS})

add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})
target_link_libraries( ${PROJECT_NAME} 
    PRIVATE 
	glog::glog
	assimp::assimp
)

# Installation
INSTALL ( TARGETS
  ${PROJECT_NAME}
  DESTINATION
  bin
  )