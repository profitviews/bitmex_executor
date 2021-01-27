# BitMEX Executor

Allows the execution of orders on the BitMEX crypto derivatives exchange.

The below describes how th build this library from scratch.  Any difficulties in doing so should be reported to the [ProfitView Slack Workspace](https://join.slack.com/t/profitviewers/shared_invite/zt-kim8mx07-WfKoRWyZmOcQq~WZizoaIA).

## Prerequisites

1. Install the compilers: 
```bash
sudo apt install build-essential
```
2. Other software packages: 
```bash
sudo apt install libssl-dev 
sudo apt install zlib1g-dev
```
3. [Boost](https://www.boost.org/doc/libs/1_75_0/)

In the directory you want to install Boost do:
```bash
wget https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.tar.bz2
tar xf boost_1_75_0.tar.bz2
cd boost_1_75_0
./bootstrap.sh
sudo ./b2 install
```
See the [Boost build docs](https://www.boost.org/doc/libs/1_75_0/more/getting_started/unix-variants.html#easy-build-and-install) if you want to change the installation locations etc.

**Note: the Boost build may take tens of minutes**
   
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
 
