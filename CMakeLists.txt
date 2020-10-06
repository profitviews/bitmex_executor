cmake_minimum_required(VERSION 3.10)

# set the project name
project(bitmex_trade_example VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(THREADS_PREFER_PTHREAD_FLAG ON)
# add the executable
add_executable(bitmex_trade_example 
    bitmex_trade_example.cpp)

find_package(Boost REQUIRED log)

find_package(OpenSSL REQUIRED
    SSL
    Crypto)

find_package(Threads REQUIRED)
    
target_link_libraries(bitmex_trade_example PUBLIC
    Boost::log
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto)

target_include_directories(bitmex_trade_example PUBLIC 
    "${PROJECT_BINARY_DIR}")
