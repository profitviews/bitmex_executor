#include "Bitmex.h"
#include "BitmexOrderExecutor.h"

std::string Bitmex::new_order(const std::string& symbol, Side side, int orderQty, OrderType type) {
    BitmexOrderExecutor executor{5, api_key_, api_secret_};
    executor.new_order(symbol, side, orderQty, type);
    return executor.result();
}
