
cmake_minimum_required(VERSION 3.15)

project(samples)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pipe -fPIC -std=c++14" CACHE string "" FORCE)

add_executable(images images.cpp)
target_link_libraries(images kmeans ${OpenCV_LIBS})

add_executable(image_to_txt image_to_txt.cpp)
target_link_libraries(image_to_txt ${OpenCV_LIBS})

add_executable(txt txt.cpp)
target_link_libraries(txt kmeans)

