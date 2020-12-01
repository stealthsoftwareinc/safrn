project(shared)

include_directories(
        ./
        ../../../../lib/fortissimo-dir/src/main/cpp
        ../../../../lib/fortissimo-dir/lib/include
)

cmake_policy(PUSH)
cmake_policy(SET CMP0015 NEW)
link_directories(
        ../../../../lib/fortissimo-dir/lib/lib/
)
cmake_policy(POP)

add_library(shared
        #JSON/Config/DatabaseConfig.cpp
        JSON/Config/ServerConfig.cpp
        JSON/Config/StudyConfig.cpp
        JSON/Columns/BoolColumn.cpp
        JSON/Columns/CategoricalColumn.cpp
        JSON/Columns/ColumnBase.cpp
        JSON/Columns/ColumnFactory.cpp
        JSON/Columns/IntegerColumn.cpp
        JSON/Columns/RealColumn.cpp
        JSON/Query/Query.cpp
        JSON/Query/SafrnFunction.cpp
        JSON/Query/FunctionFactory.cpp
        JSON/Query/MomentFunction.cpp
        JSON/Query/OrderFunction.cpp
        JSON/Query/LinearRegressionFunction.cpp
        JSON/Query/JoinStatement.cpp
        JSON/Query/ColumnSpec.cpp
        JSON/Query/Comparison.cpp
        JSON/Query/JoinOn.cpp
        JSON/Query/QueryFilter.cpp
        JSON/Query/RationalFraction.cpp
        JSON/Query/FilterMultinomialTerm.cpp
        JSON/Query/FilterMultinomial.cpp
        JSON/Query/FilterTerm.cpp
        JSON/Query/FilterExpression.cpp
        JSON/Query/GCDReducedFraction.cpp
        EventWrapper/evwrapper.cpp
        EventWrapper/evwrapper.h
        # EventWrapper/EvBufferWrapper.cpp
        # EventWrapper/EvBufferWrapper.h
        Util/Utils.cpp
        Util/StringUtils.cpp
        Util/read_file_utils.cpp
        Util/string_utils.cpp
        PeerSet.cpp
        Identity.cpp
        JSON/Query/JoinType.cpp
        JSON/Query/MomentType.cpp
        Util/read_file_utils.cpp
        Util/string_utils.cpp
)

list(APPEND shared_LIBRARIES
        fortissimo
        sst
        event_openssl
        event
        ssl
        crypto)

if (MINGW)
    list(APPEND shared_LIBRARIES
         ws2_32
         iphlpapi
        )
endif ()

target_link_libraries(shared
     ${shared_LIBRARIES}
     )

list(APPEND paulscode_shared_SOURCES
        Util/char_casting_utils.cpp
        Util/constants.cpp
        Util/data_structures.cpp
        Util/init_utils.cpp
        Util/map_utils.cpp
        Util/random_utils.cpp
        Util/read_file_utils.cpp
        Util/simulate_data_utils.cpp
        Util/socket.cpp
        Util/socket_utils.cpp
        Util/string_utils.cpp
        Util/timer_utils.cpp
        )

if (MINGW)
    list(APPEND paulscode_shared_SOURCES
         Util/windows_random_utils.cpp
         Util/windows_socket.cpp
        )
else ()
    list(APPEND paulscode_shared_SOURCES
         Util/linux_random_utils.cpp
         Util/linux_socket.cpp
        )
endif ()

add_library(paulscode_shared
            ${paulscode_shared_SOURCES}
           )

target_link_libraries(paulscode_shared
        pthread
        nettle
        fortissimo
        )

add_executable(regression_data_gen
        Util/generate_sample_data_main.cpp
        )

target_link_libraries(regression_data_gen
        paulscode_shared
        )

add_executable(regression_F_T_table_gen
	Util/generate_F_T_table.cpp
	)

target_link_libraries(regression_F_T_table_gen
  shared
	paulscode_shared
	gsl
	gslcblas
	)
