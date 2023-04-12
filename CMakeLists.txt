cmake_minimum_required(VERSION 3.0)
project(client)
set(CMAKE_CXX_STANDARD 17)
file(GLOB
            CPPS "*.cpp")


include(FetchContent)
set(SPDLOG_DIR ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
set(SPDLOG_VERSION v1.9.2) # Версия библиотеки spdlog

# Загружаем библиотеку spdlog с использованием FetchContent
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/spdlog
  GIT_TAG v1.9.2 # Измените на нужную версию
)
FetchContent_MakeAvailable(spdlog)



add_executable(${PROJECT_NAME} ${CPPS})
target_link_libraries(${PROJECT_NAME} pthread spdlog::spdlog)
