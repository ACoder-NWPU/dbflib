cmake_minimum_required(VERSION 3.12)
project(DBaseFileTools)

include_directories(include)

file(GLOB SOURCES include/*.hpp)

list(APPEND SOURCES example/main.cpp)

# 生成可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})
