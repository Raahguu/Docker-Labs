# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#


cmake_minimum_required (VERSION 3.11)
project ("Docker Labs")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()



# Include sub-projects.

add_subdirectory ("Docker Labs Core")
add_subdirectory ("Docker Labs CLI")
add_subdirectory ("Docker Labs Web")