cmake_minimum_required(VERSION 3.14)

project(ExplorerSvgSupport LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Svg REQUIRED)
find_package(Qt6WebEngineWidgets REQUIRED)

#=================== INCLUSION OF Project Files ====================#
set(FORMS_DIR "${CMAKE_SOURCE_DIR}/forms")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/MY_PROJECT_autogen/include_$<CONFIG>")

include_directories(${FORMS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})
include_directories(${GENERATED_DIR})

file(GLOB_RECURSE SOURCES
    "${FORMS_DIR}/*.ui"
    "${FORMS_DIR}/*.qrc"
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

#=================== SETUP EXECUTABLE ====================#

# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

# Define the path to the .def file
set(DEF_FILE "${CMAKE_SOURCE_DIR}/src/ExplorerSvgSupport.def")

# Use the project name for the name of the DLL
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Link the .def file for function exports (Windows only)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/DEF:${DEF_FILE}")

# Use ${PROJECT_NAME} for includes
target_include_directories(${PROJECT_NAME} PRIVATE ${FORMS_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

#===================== ENABLE ATL SUPPORT =======================#
add_definitions(-D_ATL_NO_MODULE -D_ATL_NO_OPENGL -D_ATL_STATIC_REGISTRY)

#===================== LINKING LIBRARIES =======================#
target_link_libraries(${PROJECT_NAME} Qt6::Widgets;Qt6::Svg;Qt6::Gui;Qt6::Core ole32 oleaut32 uuid)
