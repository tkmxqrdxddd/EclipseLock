cmake_minimum_required(VERSION 3.15)
project(eclipselock)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
find_package(OpenSSL REQUIRED)

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

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${OPENSSL_INCLUDE_DIR}
    ${GTKMM_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GTKMM_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Add compile definitions if needed
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Add pkg-config include directories
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTKMM_LDFLAGS}) 