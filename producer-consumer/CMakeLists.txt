cmake_minimum_required(VERSION 3.16.3)

project(producer-consumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads)

add_executable(producer-consumer main.cpp)
target_link_libraries( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )

install(TARGETS producer-consumer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
