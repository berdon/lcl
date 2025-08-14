cmake_minimum_required(VERSION 3.20)
project(MyBinary)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define directories
set(LCL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(LCL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${LCL_INCLUDE_DIR} ${GTEST_INCLUDE_DIRS})

# Test target
add_executable(async_tests
        ${LCL_SOURCE_DIR}/async/task.tests.cpp
)

# Link ASIO, Paho MQTT C, Paho MQTT C++, and threading
target_link_libraries(async_tests PRIVATE
        asio
        paho-mqtt3a
        paho-mqttpp3
        Threads::Threads
        GTest::gtest
        GTest::gtest_main
)

enable_testing()
add_test(NAME AsyncTests COMMAND async_tests)