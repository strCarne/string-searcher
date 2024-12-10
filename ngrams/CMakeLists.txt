cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(ngrams)

file(GLOB LIB_SOURCE CONFIGURE_DEPENDS "src/*.cpp")
include_directories(include)

add_library(ngrams STATIC ${LIB_SOURCE})