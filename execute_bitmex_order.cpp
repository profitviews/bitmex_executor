#include "BitmexOrderExecutor.h"
#include "Bitmex.h"

#include <boost/log/trivial.hpp>

#include <iostream>

int main(int argc, char** argv)
{
    auto usage{ [&]()->int {
        std::cout 
            << "Usage: " << argv[0] 
            << " time_to_expiry"
            << std::endl;
        return 1;
    }};

    if(argc < 2) return usage();
    
    int time_to_expiry{0};
    try {
        time_to_expiry = std::stoi(argv[1]);
    } catch(std::invalid_argument&) {
        return usage();
    }

    BOOST_LOG_TRIVIAL(info) << "Running http test.";

    Bitmex bitmex("P0Og16NeYiyN5zjrbe8C7yik", "GrFJ9ZAJDVLa7dH41M0_S1c-UE4OSzMaI1H7qqdhOQP3WDUj");
    bitmex.new_order("XBTUSD", Side::sell, 1, OrderType::market);
    bitmex.new_order("XBTUSD", Side::sell, 1, OrderType::market);
    bitmex.new_order("ETHUSD", Side::buy, 1, OrderType::market);
    bitmex.new_order("XRPUSD", Side::sell, 1, OrderType::market);

    return 0;
}