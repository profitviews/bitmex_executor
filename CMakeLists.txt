cmake_minimum_required(VERSION 3.10)

# set the project name
project(bitmex_executor VERSION 1.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(Boost_DEBUG ON)

message("-------------- Checking names -------------")
message("CMAKE_SYSTEM_NAME")
message(${CMAKE_SYSTEM_NAME})
message("===========================================")


if(CMAKE_SYSTEM_NAME MATCHES "^MINGW64")
set(CMAKE_C_COMPILER "/mingw64/bin/gcc")
set(CMAKE_CXX_COMPILER "/mingw64/bin/g++")
elseif(CMAKE_SYSTEM_NAME EQUAL Linux)
set(CMAKE_C_COMPILER "/usr/bin/gcc-10")
set(CMAKE_CXX_COMPILER "/bin/g++-10")
endif()

add_compile_options(-v -Wl,--verbose)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_library(bitmex_trade STATIC
    Bitmex.cpp
    BitmexOrderExecutor.cpp)

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW64")
  target_link_libraries(bitmex_trade 
	  ws2_32)
endif()

target_link_libraries(bitmex_trade 
    Boost::json
    Boost::log
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

set(BOOST_ROOT "/usr/local/")
find_package(Boost REQUIRED json)
find_package(Boost REQUIRED log)

find_package(OpenSSL REQUIRED
    SSL
    Crypto)

find_package(Threads REQUIRED)

# add the executable
add_executable(bitmex_executor 
    execute_bitmex_order.cpp)

target_link_libraries(bitmex_executor PUBLIC
    bitmex_trade
)
