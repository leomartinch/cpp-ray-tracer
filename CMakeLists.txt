cmake_minimum_required(VERSION 4.00)
project(leo-raytracer)
set(CMAKE_CXX_STANDARD 17)
set(SOURCES
		src/main.cc
		src/mesh.cc
)
add_executable(leo-raytracer ${SOURCES})
target_include_directories(leo-raytracer PRIVATE ${CMAKE_SOURCE_DIR}/include)
