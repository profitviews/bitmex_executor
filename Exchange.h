#pragma once

#include "OrderExecutor.h"

using Side = OrderExecutor::Side;
using OrderType = OrderExecutor::OrderType;

class Exchange {
public:
    virtual void new_order(const std::string& symbol, Side side, int orderQty, OrderType type) = 0;
    virtual ~Exchange(){};
};