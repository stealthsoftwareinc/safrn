project(shared_test)

include_directories(${GTEST_INCLUDE_DIRS})

include_directories(
        ../../main/cpp
        ../../../../lib/json-dir/include
        ../../../../lib/fortissimo-dir/src/main/cpp
        ../../../../lib/fortissimo-dir/lib/include
)

cmake_policy(PUSH)
cmake_policy(SET CMP0015 NEW)
link_directories(
        ../../../../lib/fortissimo-dir/lib/lib/
)
cmake_policy(POP)

add_executable(shared_test
        # EventWrapper/EvBufferWrapper.test.cpp
        JSON/Query/ColumnSpec.test.cpp
        JSON/Query/Polynomial.test.cpp
        JSON/Query/Comparison.test.cpp
        JSON/Query/JoinStatement.test.cpp
        JSON/Query/JoinType.test.cpp
        JSON/Query/MomentType.test.cpp
        JSON/Query/OrderFunction.test.cpp
        JSON/Query/MomentFunction.test.cpp
        JSON/Query/LinearRegressionFunction.test.cpp
        JSON/Query/Query.test.cpp
        JSON/Query/RationalFraction.test.cpp
        JSON/Query/FilterMultinomialTerm.test.cpp
        JSON/Query/FilterMultinomial.test.cpp
        JSON/Query/FilterTerm.test.cpp
        JSON/Query/FilterExpression.test.cpp
        JSON/Query/QueryFilter.test.cpp
        JSON/Query/GCDReducedFraction.test.cpp
        JSON/Columns/BoolColumn.test.cpp
        JSON/Columns/CategoricalColumn.test.cpp
        JSON/Columns/IntegerColumn.test.cpp
        JSON/Columns/RealColumn.test.cpp
        JSON/Config/StudyConfig.test.cpp
        #JSON/Config/DatabaseConfig.test.cpp
        )

target_link_libraries(shared_test
  gtest
  gmock
  gtest_main
  shared
)
