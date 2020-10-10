#pragma once

#include "Exchange.h"
#include "BitmexOrderExecutor.h"

class Bitmex
{
public:
    Bitmex(const std::string& api_key, const std::string& api_secret)
    : api_key_   { api_key    }
    , api_secret_{ api_secret }
    {}
    void new_order(const std::string& symbol, Side side, int orderQty, OrderType type) {
        std::make_unique<BitmexOrderExecutor> (5, api_key_, api_secret_)->new_order(
            symbol, side, orderQty, type);
    }
private:
    std::string api_key_;
    std::string api_secret_;

};