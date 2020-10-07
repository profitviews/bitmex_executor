#include "BitmexOrderExecutor.h"

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
    
    int number_of_tests{0}, time_to_expiry{0};
    try {
        time_to_expiry = std::stoi(argv[1]);
    } catch(std::invalid_argument&) {
        return usage();
    }

    auto bitmex{std::make_shared<BitmexOrderExecutor>(5)};
    
    BOOST_LOG_TRIVIAL(info) << "Running http test.";

    using Side = OrderExecutor::Side;
    using OrderType = OrderExecutor::OrderType;
    
    bitmex->order_new("XBTUSD", Side::buy, 1, OrderType::market);
    
    return 0;
}