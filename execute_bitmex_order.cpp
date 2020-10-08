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
    
    int time_to_expiry{0};
    try {
        time_to_expiry = std::stoi(argv[1]);
    } catch(std::invalid_argument&) {
        return usage();
    }

    BOOST_LOG_TRIVIAL(info) << "Running http test.";

    using Side = OrderExecutor::Side;
    using OrderType = OrderExecutor::OrderType;
    
    auto bitmex1{std::make_shared<BitmexOrderExecutor>(5)};
    bitmex1->order_new("XBTUSD", Side::buy, 1, OrderType::market);

    auto bitmex2{std::make_shared<BitmexOrderExecutor>(5)};
    bitmex2->order_new("ETHUSD", Side::sell, 1, OrderType::market);
    
    auto bitmex3{std::make_shared<BitmexOrderExecutor>(5)};
    bitmex3->order_new("XRPUSD", Side::buy, 1, OrderType::market);

    return 0;
}