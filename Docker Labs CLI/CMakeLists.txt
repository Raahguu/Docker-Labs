# CMakeList.txt : CMake project for Docker Labs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.11)

project(DockerLabsCli)

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

message(STATUS ${DockerLabsCoreHeaders})

add_executable (labs-cli ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET labs-cli PROPERTY CXX_STANDARD 20)
endif()

find_package(Boost REQUIRED program_options)
target_link_libraries(labs-cli PRIVATE Boost::program_options)

target_include_directories(labs-cli PRIVATE ${DockerLabsCoreHeaders})
target_link_libraries(labs-cli PRIVATE ${DockerLabsCore})