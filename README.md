# bitmex_trade_example

## Windows

1. Msys
   To build on Windows you need [Msys2](https://www.msys2.org/).  Follow the instructions there.

2. Msys will give you `pacman` package manager, `g++` and other essential programs.
   Use `pacman` to get:
   `cmake`
   `mingw-w64-x86_64-boost`
   `mingw-w64-x86_64-poco`
   
3. Download the latest TA-Lib **source** tar ball from [ta-lib.org](https://www.ta-lib.org/hdr_dw.html) - currently that is `ta-lib-0.4.0-src.tar.gz`
   In some appropriate directory do
   ```bash
   tar xf ta-lib-0.4.0-src.tar.gz 
   cd ta-lib/
   ./configure
   make
   make install
   
## Linux

1. Compilers
   `sudo apt install build-essential`
   
1. Other software
   `sudo apt install libssl-dev`
   `sudo apt install zlib1g-dev`

1. Poco
   ```shell
   git clone -b master https://github.com/pocoproject/poco.git`
   cd poco
   make
   sudo make install
   ```
   
1. Boost
   Follow the [Boost instructions](https://www.boost.org/doc/libs/1_75_0/more/getting_started/unix-variants.html)
   ```shell
   wget https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.tar.bz2
   tar xf boost_1_75_0.tar.bz2
   cd boost_1_75_0
   ./bootstrap.sh
   sudo ./b2 install
   ```
   
1. TA-Lib
   Download the latest TA-Lib **source** tar ball from [ta-lib.org](https://www.ta-lib.org/hdr_dw.html) - currently that is `ta-lib-0.4.0-src.tar.gz`
   In some appropriate directory do
   ```shell
   ta-lib-0.4.0-src.tar.gz 
   cd ta-lib/
   ./configure
   make
   sudo make install
   ```
 
   
