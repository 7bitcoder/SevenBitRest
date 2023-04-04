
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(UnitTests
    ${SOURCES}
)

target_link_libraries(UnitTests PUBLIC
    SevenBitRest
    CONAN_PKG::gtest
)

gtest_discover_tests(UnitTests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    PROPERTIES TIMEOUT 10)

install(TARGETS UnitTests DESTINATION UnitTests)

if(SEVENBITREST_SHARED_LIB)
    install(TARGETS SevenBitRest DESTINATION UnitTests)
endif()