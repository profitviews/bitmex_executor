# BitMEX Executor

Allows the execution of orders on the BitMEX crypto derivatives exchange.

The below describes how th build this library from scratch.  Any difficulties in doing so should be reported to the [ProfitView Slack Workspace](https://join.slack.com/t/profitviewers/shared_invite/zt-kim8mx07-WfKoRWyZmOcQq~WZizoaIA).

## Prerequisites

Install the compilers: 
sudo apt install build-essential
Other software packages: 
sudo apt install libssl-dev 
sudo apt install zlib1g-dev
Boost
In the directory you want to install Boost do:
wget https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.tar.bz2
tar xf boost_1_75_0.tar.bz2
cd boost_1_75_0
./bootstrap.sh
sudo ./b2 install
See the Boost build docs if you want to change the installation locations etc.
Note: the Boost build may take tens of minutes. 

## Linux

1. Compilers
   `sudo apt install build-essential`
   
1. Other software
   `sudo apt install libssl-dev`
   `sudo apt install zlib1g-dev`
   
## Build

```
git clone https://github.com/profitviews/bitmex_executor.git
cd bitmex_executor/build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

This will create (with debug symbols)
* `build/bitmex_executer` which will run the code of `execute_bitmex_order.cpp`
* `build/libbitmex_trade.a` a static library containing classes to be used to execute market orders on BitMEX
 
