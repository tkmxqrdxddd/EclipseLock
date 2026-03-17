cmake_minimum_required(VERSION 3.15)
project(eclipselock VERSION 1.1.0 LANGUAGES CXX)

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

# Platform detection
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Building for Windows")
else()
    set(PLATFORM_WINDOWS FALSE)
endif()

# For MinGW cross-compilation, set paths manually BEFORE find_package
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # MinGW cross-compilation mode - use command line provided paths or defaults
    if(NOT wxWidgets_INCLUDE_DIRS)
        set(wxWidgets_INCLUDE_DIRS "/usr/x86_64-w64-mingw32/sys-root/mingw/include/wx-3.0")
    endif()
    if(NOT wxWidgets_LIBRARIES)
        set(wxWidgets_LIBRARIES
            "/usr/x86_64-w64-mingw32/sys-root/mingw/lib/libwx_mswu_core-3.0-x86_64-w64-mingw32.dll.a"
            "/usr/x86_64-w64-mingw32/sys-root/mingw/lib/libwx_mswu_adv-3.0-x86_64-w64-mingw32.dll.a"
            "/usr/x86_64-w64-mingw32/sys-root/mingw/lib/libwx_baseu-3.0-x86_64-w64-mingw32.dll.a"
            "/usr/x86_64-w64-mingw32/sys-root/mingw/lib/libwx_baseu_net-3.0-x86_64-w64-mingw32.dll.a")
    endif()
    set(wxWidgets_FOUND TRUE)

    if(NOT OPENSSL_INCLUDE_DIR)
        set(OPENSSL_INCLUDE_DIR "/usr/x86_64-w64-mingw32/sys-root/mingw/include")
    endif()
    if(NOT OPENSSL_SSL_LIBRARY)
        set(OPENSSL_SSL_LIBRARY "/usr/x86_64-w64-mingw32/sys-root/mingw/lib/libssl.dll.a")
    endif()
    if(NOT OPENSSL_CRYPTO_LIBRARY)
        set(OPENSSL_CRYPTO_LIBRARY "/usr/x86_64-w64-mingw32/sys-root/mingw/lib/libcrypto.dll.a")
    endif()
    set(OpenSSL_FOUND TRUE)
else()
    # Find required packages for native builds
    find_package(wxWidgets REQUIRED COMPONENTS core base)
    if(NOT wxWidgets_FOUND)
        message(FATAL_ERROR "wxWidgets not found. Please install wxWidgets.")
    endif()
    include(${wxWidgets_USE_FILE})

    find_package(OpenSSL REQUIRED)
    if(NOT OpenSSL_FOUND)
        message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL.")
    endif()
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

# Windows-specific configuration
if(PLATFORM_WINDOWS)
    # Add Windows resource file for version info and icon
    set(RESOURCES src/resources.rc)

    # Windows system libraries required for wxWidgets
    set(WINDOWS_LIBS
        gdi32
        comdlg32
        ole32
        uuid
        shell32
        winmm
        wsock32
        ws2_32
    )

    # Create executable with resources (for .exe icon and version info)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

    # Set output name with .exe extension
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        OUTPUT_NAME "eclipselock"
        WIN32_EXECUTABLE TRUE
    )
else()
    # Linux/Unix build
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Add wxWidgets include directories for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "MinGW cross-compilation detected, adding wxWidgets includes: ${wxWidgets_INCLUDE_DIRS}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR})
    # Add wx/setup.h include path
    if(WX_SETUP_INCLUDE_DIR)
        target_include_directories(${PROJECT_NAME} PRIVATE ${WX_SETUP_INCLUDE_DIR})
    endif()
endif()

# Link libraries
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # MinGW cross-compilation mode - use direct library paths
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${wxWidgets_LIBRARIES}
        ${OPENSSL_SSL_LIBRARY}
        ${OPENSSL_CRYPTO_LIBRARY}
        ${WINDOWS_LIBS}
    )
elseif(PLATFORM_WINDOWS)
    # Native Windows (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${wxWidgets_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
        ${WINDOWS_LIBS}
    )
else()
    # Linux/Unix build
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${wxWidgets_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Windows-specific install (includes license)
if(PLATFORM_WINDOWS)
    install(FILES LICENSE DESTINATION share/licenses/eclipselock)
endif()
