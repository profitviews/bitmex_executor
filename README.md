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
   ta-lib-0.4.0-src.tar.gz 
   cd ta-lib/
   ./configure
   make
   make install
   
   
