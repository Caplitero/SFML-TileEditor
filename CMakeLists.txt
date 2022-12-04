cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0)
message("-- ${PROJECT_SOURCE_DIR}")

include(CTest)
enable_testing()


include_directories(SYSTEM bin)
include_directories(SYSTEM Resources)
add_executable(main main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
