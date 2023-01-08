cmake_minimum_required(VERSION 3.16.3)

project(cascade-sorted LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cascade-sorted main.cpp)

install(TARGETS cascade-sorted
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
