cmake_minimum_required(VERSION 3.11)
project(chippa)

# Rerun cmake configure when adding new files
set(SOURCES 
${CMAKE_SOURCE_DIR}/src/main.cpp
${CMAKE_SOURCE_DIR}/src/helpers.cpp
${CMAKE_SOURCE_DIR}/src/database/database.cpp
${CMAKE_SOURCE_DIR}/src/terminal/formatter.cpp
${CMAKE_SOURCE_DIR}/src/terminal/cli.cpp
${CMAKE_SOURCE_DIR}/src/auth/auth.cpp
${CMAKE_SOURCE_DIR}/src/platform/common.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)