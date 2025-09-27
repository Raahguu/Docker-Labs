# CMakeList.txt : CMake project for Docker Labs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.11)

project(DockerLabsCli)

if(UNIX)
	set(DockerLabsCore "${CMAKE_BINARY_DIR}/Docker Labs Core/libDockerLabsCore.so")
elseif(WIN32)
	set(DockerLabsCore "${CMAKE_BINARY_DIR}/Docker Labs Core/DockerLabsCore.dll")
else()
	message(FATAL_ERROR "Unsupported platform.")
endif()

set(DockerLabsCoreIncludes "${CMAKE_SOURCE_DIR}/Docker Labs Core/include")


file(GLOB SOURCES "src/*.cpp")
file(GLOB INCLUDES "include/*")

add_executable (labs-cli ${SOURCES} ${INCLUDES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET labs-cli PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(labs-cli PRIVATE "include")
target_include_directories(labs-cli PRIVATE ${DockerLabsCoreIncludes})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED program_options)

target_link_libraries(labs-cli PRIVATE Boost::program_options)

target_link_libraries(labs-cli PRIVATE ${DockerLabsCore})