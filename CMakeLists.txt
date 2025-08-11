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

# Define ASIO as an interface library
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

# Define cppcoro as an interface library
add_library(cppcoro INTERFACE)
target_include_directories(cppcoro INTERFACE ${cppcoro_SOURCE_DIR}/include)

find_package(Threads REQUIRED)
target_link_libraries(asio INTERFACE Threads::Threads)

# Define directories
set(LCL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LCL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Include directories
include_directories(${LCL_INCLUDE_DIR} ${PAHO_MQTT_C_INCLUDE_DIRS})

# Source files
set(LCL_SOURCES
    ${LCL_SOURCE_DIR}/main.cpp
    ${LCL_SOURCE_DIR}/cli/CLI.cpp
    ${LCL_SOURCE_DIR}/provider/ZigbeeProvider.cpp
    ${LCL_SOURCE_DIR}/provider/MqttProvider.cpp
    ${LCL_SOURCE_DIR}/zigbee/adapter/ZStack/ZStackAdapter.cpp
    ${LCL_SOURCE_DIR}/zigbee/adapter/ZStack/ZigbeeNetworkProcessor.cpp
    ${LCL_SOURCE_DIR}/zigbee/adapter/ZStack/ZnpCommands.cpp
    ${LCL_SOURCE_DIR}/zigbee/adapter/ZStack/ZnpTypes.cpp
)
# Create executable
add_executable(lcl ${LCL_SOURCES})

# Link ASIO, Paho MQTT C, Paho MQTT C++, and threading
target_link_libraries(lcl PRIVATE asio paho-mqtt3a paho-mqttpp3 Threads::Threads)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable warnings and optimizations
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MSVC")
  target_compile_options(lcl PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install rules
install(TARGETS lcl DESTINATION bin)
install(DIRECTORY ${LCL_INCLUDE_DIR}/ DESTINATION include)