#include "BitmexOrderExecutor.h"

#include <boost/log/trivial.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <openssl/hmac.h>

#include <iostream>
#include <iomanip>

class BitMEX_HMAC
{
public:
    BitMEX_HMAC(const std::string& api_secret, const EVP_MD* evp)
    : ctx_{HMAC_CTX_new()}
    {
        HMAC_Init_ex(ctx_, api_secret.c_str(), api_secret.length(), evp, nullptr);
    }

    void Update(const std::string& data)
    {
        HMAC_Update(ctx_, reinterpret_cast<const unsigned char *>(data.c_str()), data.length());
    }

    const std::string get_hex() 
    {
        unsigned char out[EVP_MAX_MD_SIZE];
        unsigned int len;
        HMAC_Final(ctx_, out, &len);

        std::stringstream ss;
        for (int i = 0; i < len; ++i)
            ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(out[i]);
        return ss.str();
    }

    ~BitMEX_HMAC()
    {
        HMAC_CTX_free(ctx_);
    }

private:
    HMAC_CTX* ctx_;
};

const std::map<OrderExecutor::OrderType, std::string> BitmexOrderExecutor::order_type_names_{
    {OrderType::limit, "Limit"}, {OrderType::market, "Market"}
};

const std::map<OrderExecutor::Side, std::string> BitmexOrderExecutor::side_names_{
    {Side::buy, "Buy"}, {Side::sell, "Sell"}
};

BitmexOrderExecutor::BitmexOrderExecutor(int expiry, const std::string& api_key, const std::string& api_secret)
: rest_ctx_     {ssl::context::tlsv12_client           }
, rest_resolver_{net::make_strand(rest_io_context_)           }
, rest_stream_  {net::make_strand(rest_io_context_), rest_ctx_}
, api_key_      {api_key                               }
, api_secret_   {api_secret                            }
, expiry_       {expiry                                }
{
    // Set SNI Hostname (many hosts need this to handshake successfully)
    if(! SSL_set_tlsext_host_name(rest_stream_.native_handle(), BitMEX_address.c_str()))
    {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        std::cerr << "SSL error: " << ec.message() << "\n";
        return;
    }
    
    // Set up an HTTP POST request message
    post_request_.version(11); // HTTP Version 1.1
    post_request_.method(http::verb::post);
    post_request_.target("/api/v1/order");
    post_request_.set(http::field::host, BitMEX_address);
    post_request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    post_request_.set(http::field::accept, "*/*");
    post_request_.set(http::field::content_type, "application/json");
    post_request_.set(http::field::connection, "Keep-Alive");
    post_request_.set("api-key", api_key_.c_str());
    post_request_.insert("Content-Length", "");
    post_request_.insert("api-expires", "");
    post_request_.insert("api-signature", "");
}

void BitmexOrderExecutor::new_order(const std::string& symbol, Side side, int orderQty, OrderType type)
{
    order_message_ = {
    "{"
        "\"symbol\":\""  + symbol                     + "\","
        "\"ordType\":\"" + order_type_names_.at(type) + "\","
        "\"side\":\""    + side_names_.at(side)       + "\","
        "\"orderQty\":"  + std::to_string(orderQty)   +
    "}"};

    rest_resolver_.async_resolve(BitMEX_address, SSL_port,
        [&](auto ec, auto results) { REST_on_resolve(ec, results); });

    rest_io_context_.run();
}

std::string BitmexOrderExecutor::result() const
{
    return result_body_;
}

void BitmexOrderExecutor::REST_on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    beast::get_lowest_layer(rest_stream_).async_connect(results,  
        [&](auto ec, auto endpoint) { REST_on_connect(ec, endpoint); });
}

void BitmexOrderExecutor::REST_on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
{       
    rest_stream_.async_handshake(ssl::stream_base::client, [&](auto ec) { REST_market_order_on_handshake(ec); });
}

void BitmexOrderExecutor::REST_market_order_on_handshake(beast::error_code ec)
{
    std::string valid_til{std::to_string(time(0) + expiry_)};
    post_request_.set("api-expires", valid_til);

    BitMEX_HMAC hmac{api_secret_, EVP_sha256()};
    hmac.Update(
        post_request_.method_string().to_string() + 
        post_request_.target().to_string()        + 
        valid_til                                 + 
        order_message_);
    post_request_.set("api-signature", hmac.get_hex());
    post_request_.set("Content-Length", std::to_string(order_message_.length()));
    post_request_.body() = order_message_;
    
    clock_gettime(CLOCK_MONOTONIC, &start_);
    
    BOOST_LOG_TRIVIAL(info) 
        << "Number of bytes written to stream: " 
        << http::write(rest_stream_, post_request_);
    BOOST_LOG_TRIVIAL(info) 
        << "Number of bytes transferred from the stream: " 
        << http::read(rest_stream_, rest_buffer_, post_results_);
    
    result_body_ = post_results_.body();

    clock_gettime(CLOCK_MONOTONIC, &end_);
    BOOST_LOG_TRIVIAL(info) 
        << "Response time: " 
        << (end_.tv_sec  - start_.tv_sec) + ((end_.tv_nsec - start_.tv_nsec) * 1e-9);
}

BitmexOrderExecutor::~BitmexOrderExecutor()
{
}