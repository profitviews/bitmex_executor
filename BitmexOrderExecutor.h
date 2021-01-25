#pragma once

#include "OrderExecutor.h"
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>

#include <map>
#include <vector>

namespace beast     = boost::beast;         // from <boost/beast.hpp>
namespace http      = beast::http;          // from <boost/beast/http.hpp>
namespace net       = boost::asio;          // from <boost/asio.hpp>
namespace ssl       = boost::asio::ssl;     // from <boost/asio/ssl.hpp>

using     tcp       = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

class BitmexOrderExecutor : public OrderExecutor 
{
private:
    static const std::map<OrderType, std::string> order_type_names_;
    static const std::map<Side, std::string> side_names_;
    const std::string BitMEX_address{"www.bitmex.com"};
    const std::string SSL_port{"443"};

    net::io_context rest_io_context_;
    ssl::context    rest_context_;
    
    tcp::resolver rest_resolver_;
    beast::ssl_stream<beast::tcp_stream> rest_stream_;
    beast::flat_buffer rest_buffer_;

    http::request<http::string_body>  post_request_;
    http::response<http::string_body> post_results_;

    std::string order_message_;
    
    // Timing
    struct timespec start_, end_;
    
    std::string api_key_;
    std::string api_secret_;
    int expiry_;

    void REST_on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void REST_on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    void REST_market_order_on_handshake(beast::error_code ec);
    std::string HMAC_SHA256_hex(const std::string& valid_till, const std::string& order_msg);
public:
    BitmexOrderExecutor(int, const std::string& api_key, const std::string& api_secret);
    ~BitmexOrderExecutor();
    void new_order(const std::string& symbol, Side side, int orderQty, OrderType type) override;
    boost::json::object result() const;
};
