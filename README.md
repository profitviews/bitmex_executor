# BitMEX Executor

Allows the execution of orders on the BitMEX crypto derivatives exchange.

The below describes how th build this library from scratch.  Any difficulties in doing so should be reported to the [ProfitView Slack Workspace](https://join.slack.com/t/profitviewers/shared_invite/zt-kim8mx07-WfKoRWyZmOcQq~WZizoaIA).

## Prerequisites

You will need a modern C++ compiler.  I built with `10.2.0-13ubuntu1` with the C++ 20 compiler option.

To build `bitmex_executor` you must install the following packages:
* `libssl-dev`
* `zlib1g-dev`
* [Boost](https://www.boost.org/doc/libs/1_75_0/)

## Windows

1. Msys
   To build on Windows you need [Msys2](https://www.msys2.org/).  Follow the instructions there.

2. Msys will give you `pacman` package manager, `g++` and other essential programs.
   Use `pacman` to get:
   
   * `cmake`
   * `mingw-w64-x86_64-boost`
   * `mingw-w64-x86_64-poco`
   
## Linux

1. Compilers
   `sudo apt install build-essential`
   
1. Other software
   `sudo apt install libssl-dev`
   `sudo apt install zlib1g-dev`
