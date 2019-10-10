
cmake_minimum_required(VERSION 3.15)

project(kmeans)

find_package(Threads REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pipe -fPIC -std=c++14" CACHE string "" FORCE)

set(sources kmeans.cpp stuff.cpp)
add_library(kmeans SHARED ${sources})
target_link_libraries(kmeans ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(examples)

