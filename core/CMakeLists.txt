cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(safrn)

set(CMAKE_CXX_FLAGS_CI " -Werror ")
set(CMAKE_CXX_FLAGS_METRICS " -DLOG_ENABLE_TIMER -DLOG_NO_COLOR -DNDEBUG -O3 ")
set(CMAKE_CXX_FLAGS_METRICSPROFILE " -pg -DLOG_ENABLE_TIMER -DLOG_NO_COLOR -DNDEBUG ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
  -Wcast-align \
  -std=c++11 \
  ")
  # -Wall \
  # -Wextra \
  # -Wconversion \
  # ")

find_package(nlohmann_json REQUIRED PATHS /usr/lib/cmake)

add_subdirectory(lib/fortissimo-dir)
add_subdirectory(shared/src/main/cpp)
add_subdirectory(shared/src/test/cpp)
add_subdirectory(client/src/main/cpp)
add_subdirectory(examples/src/main/cpp)
add_subdirectory(server/src/main/cpp)
add_subdirectory(server/src/test/cpp)
add_subdirectory(ffnet/src/main/cpp)
