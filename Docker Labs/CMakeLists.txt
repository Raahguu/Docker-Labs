# CMakeList.txt : CMake project for Docker Labs, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.12)

project(DockerLabs)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add source to this project's executable.
add_executable (CMakeTarget 
	main.cpp
	main.h
	cloudflare_hook.cpp
	cloudflare_hook.h
	docker_hook.cpp
	docker_hook.h
)

set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
target_link_libraries(CMakeTarget PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
