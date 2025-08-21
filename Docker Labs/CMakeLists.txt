# CMakeList.txt : CMake project for Docker Labs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

project(labs-cli)

file(GLOB SOURCES "src/*.cpp")

add_executable (labs-cli ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET labs-cli PROPERTY CXX_STANDARD 20)
endif()

find_package(CURL REQUIRED)
#find_package(nlohmann_json REQUIRED)
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(labs-cli PRIVATE CURL::libcurl)
target_link_libraries(labs-cli PRIVATE nlohmann_json::nlohmann_json)
