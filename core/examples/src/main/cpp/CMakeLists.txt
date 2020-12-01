project(examples)

include_directories(
        ./
        ../../../../shared/src/main/cpp)

cmake_policy(PUSH)
cmake_policy(SET CMP0015 NEW)
link_directories(
        ../../../../lib/fortissimo-dir/lib/lib/
)
cmake_policy(POP)

add_executable(examples
        Mod2ToModpExamples.cpp
        main.cpp)

target_link_libraries(examples
        sst
        shared
        crypto)
