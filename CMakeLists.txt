cmake_minimum_required(VERSION 3.0.0)
project(labyjava VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(labyjava main.cpp)
target_link_libraries(labyjava SDL2 SDL2main GL)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)