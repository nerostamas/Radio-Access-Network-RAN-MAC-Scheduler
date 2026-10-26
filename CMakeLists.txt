cmake_minimum_required(VERSION 3.16)
project(ran_mac_scheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Export compile commands for clang-tidy / IDEs.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_SANITIZERS "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
option(ENABLE_COVERAGE "Build with gcov coverage instrumentation" OFF)
option(BUILD_TESTING "Build unit tests" ON)

add_compile_options(-Wall -Wextra -Wpedantic)

if(ENABLE_SANITIZERS)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

if(ENABLE_COVERAGE)
  add_compile_options(--coverage -O0 -g)
  add_link_options(--coverage)
endif()

# ---- Core library ----
add_library(ran_core
  src/phy/LinkAdaptation.cpp
  src/mac/HarqProcess.cpp
  src/mac/UserEquipment.cpp
  src/mac/Schedulers.cpp
  src/metrics/Kpi.cpp
  src/Simulator.cpp
)
target_include_directories(ran_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---- CLI executable ----
add_executable(ran_sim src/main.cpp)
target_link_libraries(ran_sim PRIVATE ran_core)

# ---- Tests ----
if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  add_executable(ran_tests
    tests/test_link_adaptation.cpp
    tests/test_harq.cpp
    tests/test_kpi.cpp
    tests/test_simulator.cpp
  )
  target_link_libraries(ran_tests PRIVATE ran_core GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(ran_tests)
endif()
