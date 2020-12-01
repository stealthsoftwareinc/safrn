project(ffnet)

include_directories(
  ./
  ../../../../shared/src/main/cpp
  ../../../../server/src/main/cpp
  ../../../../lib/fortissimo-dir/src/main/cpp
  ../../../../lib/fortissimo-dir/lib/include
)

cmake_policy(PUSH)
cmake_policy(SET CMP0015 NEW)
link_directories(
        ../../../../lib/fortissimo-dir/lib/lib/
)
cmake_policy(POP)

add_executable(safrnffnet
  main.cpp
)

target_link_libraries(safrnffnet
  sst
  fortissimo
  shared
  server
  ssl
  crypto
)
