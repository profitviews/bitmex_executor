#include "BitmexOrderExecutor.h"

#include <boost/log/trivial.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <openssl/hmac.h>

#include <iostream>
#include <iomanip>

const std::map<OrderExecutor::OrderType, std::string> BitmexOrderExecutor::order_type_names_{
    {OrderType::limit, "Limit"}, {OrderType::market, "Market"}
};

const std::map<OrderExecutor::Side, std::string> BitmexOrderExecutor::side_names_{
    {Side::buy, "Buy"}, {Side::sell, "Sell"}
};

BitmexOrderExecutor::BitmexOrderExecutor(int expiry, const std::string& api_key, const std::string& api_secret)
: rest_ctx_     {ssl::context::tlsv12_client           }
, rest_resolver_{net::make_strand(rest_ioc_)           }
, rest_stream_  {net::make_strand(rest_ioc_), rest_ctx_}
, api_key_      {api_key                               }
, api_secret_   {api_secret                            }
, expiry_       {expiry                                }
{
    // Set SNI Hostname (many hosts need this to handshake successfully)
    if(! SSL_set_tlsext_host_name(rest_stream_.native_handle(), "www.bitmex.com"))
    {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        std::cerr << "ssl err " << ec.message() << "\n";
        return;
    }
    
    // Set up an HTTP GET request message
    post_req_.version(11);
    post_req_.method(http::verb::post);
    std::string target{"/api/v1/order"};
    post_req_.target(target);
    post_req_.set(http::field::host, "www.bitmex.com");
    post_req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    post_req_.set(http::field::accept, "*/*");
    post_req_.set(http::field::content_type, "application/json");
    post_req_.set(http::field::connection, "Keep-Alive");
    post_req_.set("api-key", api_key_c_str_);
    post_req_.insert("Content-Length", "");
    post_req_.insert("api-expires", "");
    post_req_.insert("api-signature", "");

}

void BitmexOrderExecutor::REST_on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    beast::get_lowest_layer(rest_stream_).async_connect(results,  
        [&](auto ec, auto endpoint) { REST_on_connect(ec, endpoint); });
}

void BitmexOrderExecutor::REST_on_connect(beast::error_code ec,
                tcp::resolver::results_type::endpoint_type)
{       
    rest_stream_.async_handshake(ssl::stream_base::client, 
        [&](auto ec) { REST_market_order_on_handshake(ec); });
}

void BitmexOrderExecutor::REST_market_order_on_handshake(beast::error_code ec)
{
    int valid_till        = time(0) + expiry_;
    std::string valid_till_str = std::to_string(valid_till);
    
    post_req_.set("api-expires", valid_till_str);
    post_req_.set("api-signature", HMAC_SHA256_hex_POST(valid_till_str, order_message_));
    post_req_.set("Content-Length", std::to_string(order_message_.length()));
    post_req_.body() = order_message_;
    
    clock_gettime(CLOCK_MONOTONIC, &start_);
    
    auto number_of_bytes_written{http::write(rest_stream_, post_req_)};
    BOOST_LOG_TRIVIAL(info) << "Number of bytes written to stream: " << number_of_bytes_written;

    auto number_of_bytes_transferred{http::read(rest_stream_, rest_buffer_, post_res_)};
    BOOST_LOG_TRIVIAL(info) << "Number of bytes transferred from the stream: " << number_of_bytes_transferred;

    clock_gettime(CLOCK_MONOTONIC, &end_);
    double time_taken;
    time_taken = (end_.tv_sec  - start_.tv_sec) + ((end_.tv_nsec - start_.tv_nsec) * 1e-9);
    BOOST_LOG_TRIVIAL(info) << "response time: " << time_taken;
}

std::string BitmexOrderExecutor::HMAC_SHA256_hex_POST(const std::string& valid_till, const std::string& order_message)
{
    std::string data = "POST/api/v1/order" + valid_till + order_message;
    
    std::stringstream ss;
    unsigned int len;
    unsigned char out[EVP_MAX_MD_SIZE];
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, api_secret_c_str_, apiSecLen, EVP_sha256(), NULL);
    HMAC_Update(ctx, (unsigned char*)data.c_str(), data.length());
    HMAC_Final(ctx, out, &len);
    HMAC_CTX_free(ctx);
    
    for (int i = 0; i < len; ++i)
    {
        ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(out[i]);
    }
    return ss.str();
}

void BitmexOrderExecutor::new_order(const std::string& symbol, Side side, int orderQty, OrderType type)
{
    order_message_ = {
    "{"
        "\"symbol\":\"" + symbol + "\","
        "\"ordType\":\"" + order_type_names_.at(type) + "\","
        "\"side\":\"" + side_names_.at(side) + "\","
        "\"orderQty\":" + std::to_string(orderQty) +
    "}"};

    rest_resolver_.async_resolve("www.bitmex.com", "443",
        [&](auto ec, auto results) { REST_on_resolve(ec, results); });

    rest_ioc_.run();
}

BitmexOrderExecutor::~BitmexOrderExecutor()
{
}