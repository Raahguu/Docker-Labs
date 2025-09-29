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

add_executable (DockerLabsCLI ${SOURCES} ${INCLUDES})
set_target_properties(DockerLabsCLI PROPERTIES OUTPUT_NAME "labs-cli")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DockerLabsCLI PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(DockerLabsCLI PRIVATE "include")
target_include_directories(DockerLabsCLI PRIVATE ${DockerLabsCoreIncludes})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED program_options)

target_link_libraries(DockerLabsCLI PRIVATE Boost::program_options)

target_link_libraries(DockerLabsCLI PRIVATE ${DockerLabsCore})

install(TARGETS DockerLabsCLI 
	RUNTIME DESTINATION bin
)