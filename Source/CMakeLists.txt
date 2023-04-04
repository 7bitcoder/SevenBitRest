file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

if(SEVENBITREST_STATIC_LIB)
  add_library(SevenBitRest STATIC ${SOURCES})
elseif(SEVENBITREST_SHARED_LIB)
  add_library(SevenBitRest SHARED ${SOURCES})
endif()

if(NOT SEVENBITREST_HEADER_OLNY_LIB)
  target_link_libraries(SevenBitRest PUBLIC
    CONAN_PKG::boost
    CONAN_PKG::openssl
    CONAN_PKG::zlib
    CONAN_PKG::certify
  )

  IF(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    find_library(SECURITY_LIBRARY Security)

    target_link_libraries(SevenBitRest PUBLIC
      ${COREFOUNDATION_LIBRARY}
      ${SECURITY_LIBRARY}
    )
  ENDIF(APPLE)

  target_include_directories(SevenBitRest PUBLIC
    .
    ../Include
    ../BoostUrl/url/include
  )

  install(TARGETS SevenBitRest DESTINATION Library)
endif()

install(DIRECTORY ../Include DESTINATION Library)

if(SEVENBITREST_HEADER_OLNY_LIB)
  install(DIRECTORY ../Source DESTINATION Library/Impl)
endif()