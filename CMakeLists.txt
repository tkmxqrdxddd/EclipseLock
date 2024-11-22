cmake_minimum_required(VERSION 3.15)
project(eclipselock)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})
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
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${wxWidgets_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Add compile definitions if needed
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 