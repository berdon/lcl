# Tags: build
cmake_minimum_required(VERSION 3.26)
project(lcl CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch ASIO using FetchContent
include(FetchContent)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG master
)
FetchContent_MakeAvailable(asio)

# Fetch Paho MQTT C using FetchContent
FetchContent_Declare(
    paho-mqtt-c
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
    GIT_TAG v1.3.13
)
set(PAHO_WITH_SSL FALSE CACHE BOOL "Disable SSL for Paho MQTT C")
set(PAHO_BUILD_STATIC TRUE CACHE BOOL "Build static Paho MQTT C library")
FetchContent_MakeAvailable(paho-mqtt-c)

# Set PahoMqttC paths explicitly
set(PAHO_MQTT_C_INCLUDE_DIRS ${paho-mqtt-c_SOURCE_DIR}/src)
set(PAHO_MQTT_C_LIBRARIES ${paho-mqtt-c_BINARY_DIR}/src/libpaho-mqtt3a.a)
set(PahoMqttC_FOUND TRUE)

# Debug output to verify paths
message(STATUS "Paho MQTT C Include Dirs: ${PAHO_MQTT_C_INCLUDE_DIRS}")
message(STATUS "Paho MQTT C Libraries: ${PAHO_MQTT_C_LIBRARIES}")

# Fetch Paho MQTT C++ using FetchContent
FetchContent_Declare(
    paho-mqtt-cpp
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.cpp.git
    GIT_TAG v1.3.2
    GIT_SUBMODULES "" # Disable submodules to avoid paho-mqtt-c submodule conflict
)
FetchContent_MakeAvailable(paho-mqtt-cpp)

# Enable testing
enable_testing()

find_package(GTest QUIET)
if (GTest_FOUND)
    message(STATUS "Found system-installed Google Test")
else()
    # Option 2: Use FetchContent to download Google Test
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1  # Use a stable release
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Define ASIO as an interface library
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

# Define cppcoro as an interface library
add_library(cppcoro INTERFACE)
target_include_directories(cppcoro INTERFACE ${cppcoro_SOURCE_DIR}/include)

find_package(Threads REQUIRED)
target_link_libraries(asio INTERFACE Threads::Threads)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)