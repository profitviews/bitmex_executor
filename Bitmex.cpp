#include "Bitmex.h"

#include <boost/json.hpp>
#include "BitmexOrderExecutor.h"

boost::json::object
Bitmex::new_order(const std::string& symbol, Side side, int orderQty, OrderType type) {
    BitmexOrderExecutor executor{5, api_key_, api_secret_};
    executor.new_order(symbol, side, orderQty, type);
    return executor.result();
}
