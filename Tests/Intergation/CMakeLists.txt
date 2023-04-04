
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(IntegrationTests
    ${SOURCES}
)

target_link_libraries(IntegrationTests PUBLIC
    SevenBitRest
    CONAN_PKG::gtest
)

gtest_discover_tests(IntegrationTests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    PROPERTIES TIMEOUT 10)

install(TARGETS IntegrationTests DESTINATION IntegrationTests)

if(SEVENBITREST_SHARED_LIB)
    install(TARGETS SevenBitRest DESTINATION IntegrationTests)
endif()