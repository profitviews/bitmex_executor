#pragma once

#include <string>

class OrderExecutor
{
public:
    OrderExecutor(){}
    virtual ~OrderExecutor(){}
    enum class Side { buy, sell };
    enum class OrderType { limit, market };
    virtual void order_new(const std::string& symbol, Side side, int orderQty, OrderType type) = 0;
};