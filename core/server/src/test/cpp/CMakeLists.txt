project(server_test)

include_directories(${GTEST_INCLUDE_DIRS})

include_directories(
  ./
  ../../main/cpp
  ../../../../shared/src/main/cpp
  ../../../../lib/json-dir/include
  ../../../../lib/fortissimo-dir/src/main/cpp
  ../../../../lib/fortissimo-dir/lib/include/
)

cmake_policy(PUSH)
cmake_policy(SET CMP0015 NEW)
link_directories(
        ../../../../lib/fortissimo-dir/lib/lib/
)
cmake_policy(POP)

add_executable(server_test
  QueryTester.h
  QueryTester.cpp
  Regression.test.cpp
  Lookup.test.cpp
  Moments.test.cpp
#  ConditionalEvaluate.test.cpp
  dataowner/lagrange.test.cpp
  dealer/RandomSquareMatrix.test.cpp
  Startup.test.cpp
)

target_link_libraries(server_test
  sst
  fortissimo
  shared
  gtest
  gmock
  gtest_main
  server
  ssl
  crypto
)
