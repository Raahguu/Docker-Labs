
cmake_minimum_required (VERSION 3.11)

project(DockerLabsCore VERSION 1.0.0 DESCRIPTION "Core library for the Docker Labs project.")

file(GLOB SOURCES "src/*.cpp")
file(GLOB INCLUDES "include/*")


add_library(DockerLabsCore SHARED ${SOURCES} ${INCLUDES})
target_include_directories(DockerLabsCore PRIVATE "include")

set_target_properties(DockerLabsCore PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(DockerLabsCore PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DockerLabsCore PROPERTY CXX_STANDARD 20)
endif()

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(DockerLabsCore PRIVATE CURL::libcurl)
target_link_libraries(DockerLabsCore PRIVATE nlohmann_json::nlohmann_json)