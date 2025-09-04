# CMakeList.txt : CMake project for Docker Labs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

project(DockerLabsCli)

set(DockerLabsCore "${CMAKE_CURRENT_BINARY_DIR}/../Docker Labs Core/libDockerLabsCore.so")
set(DockerLabsCoreHeaders "${CMAKE_CURRENT_BINARY_DIR}/../Docker Labs Core/include")
file(GLOB SOURCES "src/*.cpp")

message(STATUS ${DockerLabsCore})


add_executable (labs-cli ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET labs-cli PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(labs-cli PRIVATE ${DockerLabsCoreHeaders})

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(labs-cli PRIVATE ${DockerLabsCore})
target_link_libraries(labs-cli PRIVATE CURL::libcurl)
target_link_libraries(labs-cli PRIVATE nlohmann_json::nlohmann_json)
