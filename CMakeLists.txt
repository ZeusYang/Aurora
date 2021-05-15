cmake_minimum_required (VERSION 3.5)

project(Aurora)

# C++ 11 is required
set(CMAKE_CXX_STANDARD 11)

OPTION(AURORA_DOUBLE_AS_FLOAT "Use 64-bit floats" OFF)

IF (AURORA_DOUBLE_AS_FLOAT)
  ADD_DEFINITIONS ( -D AURORA_DOUBLE_AS_FLOAT )
ENDIF()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(src/accelerators/)
include_directories(src/cameras/)
include_directories(src/core/)
include_directories(src/filters/)
include_directories(src/integrators/)
include_directories(src/materials/)
include_directories(src/samplers/)
include_directories(src/shapes/)
include_directories(src/textures/)


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

file(GLOB ACCELECTORS ./src/accelerators/*.h ./src/accelerators/*.cpp)
source_group("accelectors" FILES ${ACCELECTORS})

file(GLOB CAMERAS ./src/cameras/*.h ./src/cameras/*.cpp)
source_group("cameras" FILES ${CAMERAS})

file(GLOB CORE ./src/core/*.h ./src/core/*.cpp)
source_group("core" FILES ${CORE})

file(GLOB FILTERS ./src/filters/*.h ./src/filters/*.cpp)
source_group("filters" FILES ${FILTERS})

file(GLOB INTEGRATORS ./src/integrators/*.h ./src/integrators/*.cpp)
source_group("integrators" FILES ${INTEGRATORS})

file(GLOB MATERIALS ./src/materials/*.h ./src/materials/*.cpp)
source_group("materials" FILES ${MATERIALS})

file(GLOB SAMPLERS ./src/samplers/*.h ./src/samplers/*.cpp)
source_group("samplers" FILES ${SAMPLERS})

file(GLOB SHAPES ./src/shapes/*.h ./src/shapes/*.cpp)
source_group("shapes" FILES ${SHAPES})

file(GLOB LIGHTS ./src/lights/*.h ./src/lights/*.cpp)
source_group("lights" FILES ${LIGHTS})

file(GLOB TEXTURES ./src/textures/*.h ./src/textures/*.cpp)
source_group("textures" FILES ${TEXTURES})

file(GLOB SRCS ./src/*.cpp)

add_executable(${PROJECT_NAME} ${ACCELECTORS} ${CAMERAS} ${CORE} ${FILTERS}
		 ${INTEGRATORS} ${MATERIALS} ${SAMPLERS} ${SHAPES} ${LIGHTS} ${TEXTURES} ${SRCS})

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