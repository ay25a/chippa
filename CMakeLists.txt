cmake_minimum_required(VERSION 3.11)
project(chippa)

# Rerun cmake configure when adding new files
set(SOURCES 
${CMAKE_SOURCE_DIR}/src/main.cpp
${CMAKE_SOURCE_DIR}/src/database/database.cpp
${CMAKE_SOURCE_DIR}/src/database/user.cpp
${CMAKE_SOURCE_DIR}/src/terminal/cli.cpp
${CMAKE_SOURCE_DIR}/src/ui/auth.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)