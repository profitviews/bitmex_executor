#pragma once

#include <boost/json.hpp>
#include "Exchange.h"

class Bitmex : public Exchange
{
public:
    Bitmex(const std::string& api_key, const std::string& api_secret)
    : api_key_   { api_key    }
    , api_secret_{ api_secret }
    {}
    boost::json::object new_order(const std::string& symbol, Side side, int orderQty, OrderType type) override;

private:
    std::string api_key_;
    std::string api_secret_;
};