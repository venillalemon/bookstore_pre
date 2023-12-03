cmake_minimum_required(VERSION 3.22)
project(bookstore_pre)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(bookstore_pre main.cpp
        src/books.h)
