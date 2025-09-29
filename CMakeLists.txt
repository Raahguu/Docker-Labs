# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#


cmake_minimum_required (VERSION 3.11)
project ("Docker Labs" VERSION 0.1)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()



# Include sub-projects.

add_subdirectory ("Docker Labs Core")
add_subdirectory ("Docker Labs CLI")

set(PACKAGE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/package")

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/DockerLabsConfig.cmake"
    INSTALL_DESTINATION lib/cmake/DockerLabs
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DockerLabsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

include(CPack)

set(CPACK_GENERATOR "TGZ;ZIP")  # Specify the package formats
set(CPACK_PACKAGE_NAME "DockerLabs")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "DockerLabs package containing Core and CLI.")
set(CPACK_PACKAGE_VENDOR "STRDEV")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "DockerLabs")
set(CPACK_PACKAGE_EXECUTABLES "DockerLabsCore;DockerLabsCore")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DockerLabsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DockerLabsConfigVersion.cmake"
    DESTINATION lib/cmake/DockerLabs
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/scripts/install.sh"
        DESTINATION ".")  