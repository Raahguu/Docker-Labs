
cmake_minimum_required (VERSION 3.10)

project(DockerLabsCore)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")
set(HEADER_DEST "include")

#message(STATUS "lalalall" ${SOURCES} ${HEADERS})

add_library(DockerLabsCore SHARED ${SOURCES} ${HEADERS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DockerLabsCore PROPERTY CXX_STANDARD 20)
endif()

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

file(MAKE_DIRECTORY ${HEADER_DEST})
#make_directory(${HEADER_DEST})

add_custom_command(TARGET DockerLabsCore POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
  ${HEADER_DEST}
)

add_custom_command(TARGET DockerLabsCore POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${HEADERS} ${HEADER_DEST}
)

target_link_libraries(DockerLabsCore PRIVATE CURL::libcurl)
target_link_libraries(DockerLabsCore PRIVATE nlohmann_json::nlohmann_json)