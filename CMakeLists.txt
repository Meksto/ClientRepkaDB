cmake_minimum_required(VERSION 3.0)
project(client)
set(CMAKE_CXX_STANDARD 17)
file(GLOB
            CPPS "*.cpp")
add_executable(${PROJECT_NAME} ${CPPS})
target_link_libraries(${PROJECT_NAME} pthread)
