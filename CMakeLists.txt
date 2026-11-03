cmake_minimum_required(VERSION 3.20)
project(double_pendulum_simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 REQUIRED COMPONENTS Graphics)
find_package(GSL REQUIRED)

add_executable(double_pendulum src/main.cpp)
target_link_libraries(double_pendulum PRIVATE SFML::Graphics GSL::gsl GSL::gslcblas)
