cmake_minimum_required(VERSION 3.15)
project(eclipselock VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
    add_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find required packages
find_package(wxWidgets REQUIRED COMPONENTS core base)
if(NOT wxWidgets_FOUND)
    message(FATAL_ERROR "wxWidgets not found. Please install wxWidgets.")
endif()
include(${wxWidgets_USE_FILE})

find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL.")
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

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${wxWidgets_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
