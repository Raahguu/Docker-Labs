
cmake_minimum_required (VERSION 3.11)

project(DockerLabsCore)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h" "src/*.hpp")
set(HEADER_DEST "${CMAKE_BINARY_DIR}/Docker Labs Core/include")


#message(STATUS "lalalall" ${SOURCES} ${HEADERS})

add_library(DockerLabsCore SHARED ${SOURCES} ${HEADERS})

file(MAKE_DIRECTORY ${HEADER_DEST})

#foreach(HEADER_FILE ${HEADER_FILES})
#    get_filename_component(HEADER_NAME ${HEADER_FILE} NAME)  # Get the file name
#    configure_file(${HEADER_FILE} "${HEADER_DEST}/${HEADER_NAME}" COPYONLY)
#endforeach()

add_custom_command(TARGET DockerLabsCore
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${HEADER_DEST}
  COMMAND ${CMAKE_COMMAND} -E make_directory
  ${HEADER_DEST}
  COMMAND ${CMAKE_COMMAND} -E copy
  ${HEADERS} ${HEADER_DEST}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DockerLabsCore PROPERTY CXX_STANDARD 20)
endif()


find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

#file(MAKE_DIRECTORY ${HEADER_DEST})

target_link_libraries(DockerLabsCore PUBLIC CURL::libcurl)
target_link_libraries(DockerLabsCore PUBLIC nlohmann_json::nlohmann_json)