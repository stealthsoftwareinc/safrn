project(client)

include_directories(
        ../../../../lib/fortissimo-dir/src/main/cpp
        ../../../../lib/fortissimo-dir/lib/include
        ../../../../lib/json-dir/include
        ../../../../server/src/main/cpp/
        ../../../../shared/src/main/cpp/
        ../../../../shared/src/main/cpp/EventWrapper
        ../../../../shared/src/main/cpp/Util
        ../../../../shared/src/main/cpp/JSON
        ../../../../shared/src/main/cpp/JSON/Config
        ../../../../shared/src/main/cpp/JSON/Columns)

cmake_policy(PUSH)
cmake_policy(SET CMP0015 NEW)
link_directories(
        ../../../../lib/fortissimo-dir/lib/lib/
)
cmake_policy(POP)

add_executable(client_send_query
        send_query_main.cpp)

target_link_libraries(client_send_query
        paulscode_shared
        server
        shared)
