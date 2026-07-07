cmake_minimum_required(VERSION 3.15)
project(eclipselock)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(PkgConfig QUIET)
find_package(OpenSSL REQUIRED)

# Optional GTKmm for GUI
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(GTKMM gtkmm-3.0)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/gui/MainWindow.cpp
    src/AESCrypt.cpp
)

# Header files
set(HEADERS
    src/gui/MainWindow.h
    src/AESCrypt.h
)

# Create executable (GUI only if GTKmm is available)
if(GTKMM_FOUND)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${OPENSSL_INCLUDE_DIR}
        ${GTKMM_INCLUDE_DIRS}
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${GTKMM_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
        ${GTKMM_LDFLAGS}
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    )

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )
else()
    message(STATUS "gtkmm-3.0 not found — GUI binary will not be built")
endif()

# Testing
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(Catch2)

    set(TEST_SOURCES
        tests/test_aescrypt.cpp
    )

    add_executable(eclipselock_tests ${TEST_SOURCES} src/AESCrypt.cpp)

    target_include_directories(eclipselock_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${OPENSSL_INCLUDE_DIR}
    )

    target_link_libraries(eclipselock_tests PRIVATE
        Catch2::Catch2WithMain
        OpenSSL::SSL
        OpenSSL::Crypto
    )

    add_test(NAME eclipselock_tests COMMAND eclipselock_tests)
endif()
