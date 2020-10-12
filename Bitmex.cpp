#include "Bitmex.h"

void Bitmex::new_order(const std::string& symbol, Side side, int orderQty, OrderType type) {
    BitmexOrderExecutor(5, api_key_, api_secret_).new_order(symbol, side, orderQty, type);
}
