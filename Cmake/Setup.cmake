set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/publish)

SET(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH}:\$ORIGIN/../bin:\$ORIGIN)

IF(WIN32)
    ADD_DEFINITIONS(/bigobj)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
    SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /bigobj")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
ENDIF(WIN32)

set(BUILD_LIBRARY_TYPE "Shared" CACHE STRING
    "Library build type: Shared;Static;HeaderOnly")

set(BUILD_LIBRARY_TYPE_VALUES "Shared;Static;HeaderOnly" CACHE STRING
    "List of possible BUILD_LIBRARY_Type values")

set_property(CACHE BUILD_LIBRARY_TYPE PROPERTY STRINGS ${BUILD_LIBRARY_TYPE_VALUES})

message(STATUS "===== Library build as ${BUILD_LIBRARY_TYPE} library =====")

set(BUILD_UNIT_TESTS OFF CACHE BOOL "Turn on to build unit tests")
set(BUILD_INTEGRATION_TESTS OFF CACHE BOOL "Turn on to build integration tests")
set(BUILD_E2E_TESTS OFF CACHE BOOL "Turn on to build e2e tests")
set(BUILD_EXAMPLES OFF CACHE BOOL "Turn on to build examples")
set(BUILD_BENCHMARKS OFF CACHE BOOL "Turn on to build benchmarks")

if(BUILD_LIBRARY_TYPE STREQUAL "Shared")
    set(SEVENBITREST_SHARED_LIB ON)
elseif(BUILD_LIBRARY_TYPE STREQUAL "Static")
    set(SEVENBITREST_STATIC_LIB ON)
elseif(BUILD_LIBRARY_TYPE STREQUAL "HeaderOnly")
    set(SEVENBITREST_HEADER_OLNY_LIB ON)
endif()

set(SEVENBITREST_VERSION ${CMAKE_PROJECT_VERSION})

configure_file(Include/Common/LibraryConfig.hpp.input ${PROJECT_SOURCE_DIR}/Include/Common/LibraryConfig.hpp)