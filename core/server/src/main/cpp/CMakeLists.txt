project(server)

include_directories(
  ./
  ../../../../shared/src/main/cpp
  ../../../../lib/fortissimo-dir/src/main/cpp
  ../../../../lib/fortissimo-dir/lib/include/
)

cmake_policy(PUSH)
cmake_policy(SET CMP0015 NEW)
link_directories(
        ../../../../lib/fortissimo-dir/lib/lib/
)
cmake_policy(POP)

add_library(server
  # network/NewConnectionHandler.cpp
  # network/NewConnectionHandler.h
  # network/EventHandler.cpp
  # network/EventHandler.h
  # framework/QueryContext.cpp
  # framework/QueryContext.h
  # framework/QueryHandler.cpp
  # framework/QueryHandler.h
  dataowner/Regression.h
  dataowner/Regression.cpp
  dataowner/RegressionInfo.h
  dataowner/RegressionInfo.cpp
  dataowner/RegressionPatron.h
  dataowner/RegressionPatron.cpp
  dataowner/RegressionPayloadCompute.h
  dataowner/RegressionPayloadCompute.cpp
  dataowner/Lookup.h
  dataowner/Lookup.cpp
  dataowner/LookupPatron.h
  dataowner/LookupPatron.cpp
  recipient/RegressionReceiver.h
  recipient/RegressionReceiver.cpp
  dataowner/Moments.h
  dataowner/Moments.cpp
  dataowner/MomentsInfo.h
  dataowner/MomentsInfo.cpp
  dataowner/MomentsPatron.h
  dataowner/MomentsPatron.cpp
  recipient/MomentsReceiver.h
  recipient/MomentsReceiver.cpp
  dataowner/GlobalInfo.h
  dataowner/GlobalInfo.cpp
  dataowner/Share.h
  dataowner/Share.cpp
  dataowner/fortissimo.h
  dataowner/lagrange.h
  dataowner/lagrange.cpp
  dealer/RandomTableLookup.h
  dealer/RandomTableLookup.t.h
  dealer/RandomTableLookup.cpp
  dealer/LookupHouse.h
  dealer/LookupHouse.cpp
  dealer/RandomSquareMatrix.h
  dealer/RandomSquareMatrix.t.h
  dealer/RegressionHouse.h
  dealer/RegressionHouse.cpp
  dealer/MomentsHouse.h
  dealer/MomentsHouse.cpp
  framework/Framework.h
  framework/TestRunner.h
  framework/TestRunner.cpp
  util/Randomness.h
  util/RandomnessDealer.h

  Startup.h
  Startup.cpp
  StartupUtils.h
  StartupUtils.cpp
  StartupRegression.h
  StartupRegression.cpp
  StartupMoments.h
  StartupMoments.cpp
  )

target_link_libraries(server
  sst
  fortissimo
  shared
  ssl
  crypto)

add_executable(server_receive_query
        receive_query_main.cpp)

target_link_libraries(server_receive_query
        paulscode_shared
        server
        shared)
