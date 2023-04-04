
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(E2E
    ${SOURCES}
)

target_link_libraries(E2E PUBLIC
    SevenBitRest
    CONAN_PKG::gtest
)

gtest_discover_tests(E2E
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    PROPERTIES TIMEOUT 10)

install(TARGETS E2E DESTINATION E2E)

if(SEVENBITREST_SHARED_LIB)
    install(TARGETS SevenBitRest DESTINATION E2E)
endif()