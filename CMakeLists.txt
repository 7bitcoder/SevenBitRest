cmake_minimum_required(VERSION 3.0.0)
project(7BitRest VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/Cmake)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

include(GoogleTest)
include(CPack)
enable_testing()

include(Setup)
add_subdirectory(Source)

if(BUILD_UNIT_TESTS)
        add_subdirectory(Tests/Unit)
endif()

if(BUILD_INTEGRATION_TESTS)
        add_subdirectory(Tests/Intergation)
endif()

if(BUILD_E2E_TESTS)
        add_subdirectory(Tests/E2E)
endif()

if(BUILD_BENCHMARKS)
        add_subdirectory(Benchmarks)
endif()

if(BUILD_EXAMPLES)
        add_subdirectory(Examples)
endif()