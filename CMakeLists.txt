cmake_minimum_required(VERSION 3.11)
project(chippa)

set(CMAKE_CXX_STANDARD 14)
# Rerun cmake configure when adding new files
set(SOURCES 
${CMAKE_SOURCE_DIR}/src/main.cpp
${CMAKE_SOURCE_DIR}/src/ui_elements.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)