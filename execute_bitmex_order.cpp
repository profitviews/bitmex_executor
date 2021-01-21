#include "BitmexOrderExecutor.h"
#include "Bitmex.h"

#include <boost/log/trivial.hpp>

#include <iostream>

auto main(int argc, char** argv) -> int
{
    if(argc < 3) 
    {
        std::cout 
            << "Usage: " << argv[0] 
            << " API_key API_secret"
            << std::endl;
        return 1;
    }

    BOOST_LOG_TRIVIAL(info) << "Running Bitmex test.";

    Bitmex bitmex(argv[1], argv[2]);
    BOOST_LOG_TRIVIAL(info) << "XBTUSD 1 result: " << bitmex.new_order("XBTUSD", Side::sell, 1, OrderType::market) << std::endl;
    BOOST_LOG_TRIVIAL(info) << "XBTUSD 2 result: " << bitmex.new_order("XBTUSD", Side::sell, 1, OrderType::market) << std::endl;
    BOOST_LOG_TRIVIAL(info) << "ETHUSD result: " << bitmex.new_order("ETHUSD", Side::buy,  1, OrderType::market) << std::endl;
    BOOST_LOG_TRIVIAL(info) << "XRPUSD result: " << bitmex.new_order("XRPUSD", Side::sell, 1, OrderType::market) << std::endl;

    return 0;
}