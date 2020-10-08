#pragma once

#include "OrderExecutor.h"
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio/ip/tcp.hpp>

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

    net::io_context rest_ioc_;
    ssl::context    rest_ctx_;
    
    tcp::resolver rest_resolver_;
    beast::ssl_stream<beast::tcp_stream> rest_stream_;
    beast::flat_buffer rest_buffer_;

    http::request<http::string_body>  post_req_;
    http::response<http::string_body> post_res_;

    std::string order_message_;
    
    // Timing
    struct timespec start_, end_;
    
    std::string api_key_    = "P0Og16NeYiyN5zjrbe8C7yik";
    std::string api_secret_ = "GrFJ9ZAJDVLa7dH41M0_S1c-UE4OSzMaI1H7qqdhOQP3WDUj";
    int    api_key_length_ = api_key_.length();
    const char* api_key_c_str_ = api_key_.c_str();
    int    apiSecLen = api_secret_.length();
    const char* api_secret_c_str_ = api_secret_.c_str();
    int    expiry_;

    void REST_on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void REST_on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    void REST_market_order_on_handshake(beast::error_code ec);
    std::string HMAC_SHA256_hex_POST_single(const std::string& valid_till, const std::string& order_msg);
public:
    BitmexOrderExecutor(int);
    ~BitmexOrderExecutor();
    void order_new(const std::string& symbol, Side side, int orderQty, OrderType type) override;

};