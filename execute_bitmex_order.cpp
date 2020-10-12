#include "BitmexOrderExecutor.h"
#include "Bitmex.h"

#include <boost/log/trivial.hpp>

#include <iostream>

auto main(int argc, char** argv) -> int
{
    auto usage{ [&]()->int {
        std::cout 
            << "Usage: " << argv[0] 
            << "API_key API_secret"
            << std::endl;
        return 1;
    }};

    if(argc < 3) return usage();

    BOOST_LOG_TRIVIAL(info) << "Running Bitmex test.";

    Bitmex bitmex(argv[1], argv[2]);
    bitmex.new_order("XBTUSD", Side::sell, 1, OrderType::market);
    bitmex.new_order("XBTUSD", Side::sell, 1, OrderType::market);
    bitmex.new_order("ETHUSD", Side::buy, 1, OrderType::market);
    bitmex.new_order("XRPUSD", Side::sell, 1, OrderType::market);

    return 0;
}