# Tags: build
cmake_minimum_required(VERSION 3.26)
project(lcl CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

### Lcl
# Define directories
set(LCL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(LCL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

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

# Enable warnings and optimizations
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MSVC")
    target_compile_options(lcl PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install rules
install(TARGETS lcl DESTINATION bin)
install(DIRECTORY ${LCL_INCLUDE_DIR}/ DESTINATION include)