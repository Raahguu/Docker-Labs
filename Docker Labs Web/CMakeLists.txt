# CMakeList.txt : CMake project for Docker Labs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.11)

project(DockerLabsWeb)

if(UNIX)
	set(DockerLabsCore "${CMAKE_BINARY_DIR}/Docker Labs Core/libDockerLabsCore.so")
	set(DockerLabsCoreHeaders "${CMAKE_BINARY_DIR}/Docker Labs Core/include")
elseif(WIN32)
	set(DockerLabsCore "${CMAKE_BINARY_DIR}/Docker Labs Core/DockerLabsCore.dll")
	set(DockerLabsCoreHeaders "${CMAKE_BINARY_DIR}/Docker Labs Core/include")
else()
	message(FATAL_ERROR "Unsupported platform.")
endif()

file(GLOB SOURCES "src/*.cpp")

message(STATUS ${DockerLabsCore})


add_executable (labs-web ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET labs-web PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(labs-web PRIVATE ${DockerLabsCoreHeaders})

target_link_libraries(labs-web PRIVATE ${DockerLabsCore})
