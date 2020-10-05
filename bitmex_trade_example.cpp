// g++ -std=c++17 -pthread -o http_test.out http_test.cpp -lssl -lcrypto && ./http_test.out

//Boost & Beast headers
#include <boost/bind.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/optional.hpp>

//REST headers
#include <sstream>
#include <openssl/evp.h>
#include <openssl/hmac.h>

//Misc. headers
#include <iomanip>
#include <iostream>
#include <string>

namespace beast     = boost::beast;         // from <boost/beast.hpp>
namespace http      = beast::http;          // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;     // from <boost/beast/websocket.hpp>
namespace net       = boost::asio;          // from <boost/asio.hpp>
namespace ssl       = boost::asio::ssl;     // from <boost/asio/ssl.hpp>
using     tcp       = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

using namespace std;


class BitMEX_MM : public std::enable_shared_from_this<BitMEX_MM> {
    
    int n_tests = 1;
    
    //REST
    tcp::resolver rest_resolver;
    beast::ssl_stream<beast::tcp_stream> rest_stream;
    beast::flat_buffer rest_buffer;
    
    http::request<http::string_body>  post_req;
    http::response<http::string_body> post_res;
    
    string limit_order_msg  = "{\"orders\":[";
    
    // Timing
    struct timespec start, end;
    
    //MEMBER VARIABLES
    string apiKey    = "P0Og16NeYiyN5zjrbe8C7yik";
    string apiSecret = "GrFJ9ZAJDVLa7dH41M0_S1c-UE4OSzMaI1H7qqdhOQP3WDUj";
    int    apiKeyLen = apiKey.length();
    const char* apiKeyCStr = apiKey.c_str();
    int    apiSecLen = apiSecret.length();
    const char* apiSecCStr = apiSecret.c_str();
    int    expiry_t  = 5;
    
    //REST FUNCTIONS
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    
    string HMAC_SHA256_hex_POST(string valid_till)
    {
        string data = "POST/api/v1/order/bulk" + valid_till + limit_order_msg;
        
        stringstream ss;
        unsigned int len;
        unsigned char out[EVP_MAX_MD_SIZE];
        HMAC_CTX *ctx = HMAC_CTX_new();
        HMAC_Init_ex(ctx, apiSecCStr, apiSecLen, EVP_sha256(), NULL);
        HMAC_Update(ctx, (unsigned char*)data.c_str(), data.length());
        HMAC_Final(ctx, out, &len);
        HMAC_CTX_free(ctx);
        
        for (int i = 0; i < len; ++i)
        {
            ss << std::setw(2) << std::setfill('0') << hex << (unsigned int)out[i];
        }
        return ss.str();
    }
    
    void
    REST_on_resolve(
        beast::error_code ec,
        tcp::resolver::results_type results)
    {
        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(rest_stream).async_connect(
            results,
            beast::bind_front_handler(
                &BitMEX_MM::REST_on_connect,
                shared_from_this()));
    }

    void
    REST_on_connect(beast::error_code ec,
                    tcp::resolver::results_type::endpoint_type)
    {       
        // Perform the SSL handshake
        rest_stream.async_handshake(
            ssl::stream_base::client,
            beast::bind_front_handler(
                &BitMEX_MM::REST_on_handshake,
                shared_from_this()));
    }
    
    void
    REST_on_handshake(beast::error_code ec)
    {
        limit_order_msg += 
            "{"
                "\"symbol\":\"XBTUSD\","
                "\"ordType\":\"Limit\","
                "\"execInst\":\"ParticipateDoNotInitiate\","
                "\"clOrdID\":\"" + to_string(n_tests) + "\","
                "\"side\":\"Buy\","
                "\"price\":10.0"
                ",\"orderQty\":2"
            "}]}";
        REST_write_limit_order_bulk();
    }
    
    void REST_write_limit_order_bulk()
    {
        int valid_till        = time(0) + 5;
        string valid_till_str = to_string(valid_till);
        
        post_req.set("api-expires", valid_till_str);
        post_req.set("api-signature", HMAC_SHA256_hex_POST(valid_till_str));
        post_req.set("Content-Length", to_string(limit_order_msg.length()));
        post_req.body() = limit_order_msg;
        
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        http::write(rest_stream, post_req);
        http::read(rest_stream, rest_buffer, post_res);
        
        beast::error_code _ec;
        std::size_t       _bt;
        process_limit_order_bulk_res(_ec, _bt);
    }
    
    void process_limit_order_bulk_res(beast::error_code ec,
                                      std::size_t bytes_transferred)
    {
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_taken;
        time_taken = (end.tv_sec  - start.tv_sec) + ((end.tv_nsec - start.tv_nsec) * 1e-9);
        cout << "response time: " << time_taken << endl;
        
        ++n_tests;
        
        if (n_tests <= 5)
        {
            limit_order_msg  = 
                "{"
                    "\"orders\":["
                        "{"
                            "\"symbol\":\"XBTUSD\","
                            "\"ordType\":\"Limit\","
                            "\"execInst\":\"ParticipateDoNotInitiate\","
                            "\"clOrdID\":\"" + to_string(n_tests) + "\","
                            "\"side\":\"Buy\","
                            "\"price\":10.0,"
                            "\"orderQty\":2"
                        "}"
                    "]"
                "}";
            REST_write_limit_order_bulk();
        }
    }
    
public:
        
    explicit
    BitMEX_MM(net::io_context& rest_ioc, ssl::context& rest_ctx)
        : rest_resolver(net::make_strand(rest_ioc))
        , rest_stream(net::make_strand(rest_ioc), rest_ctx)
    { }
    
    void
    run_REST_service()
    {           
        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(rest_stream.native_handle(), "www.bitmex.com"))
        {
            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            std::cerr << "ssl err " << ec.message() << "\n";
            return;
        }
        
        // Set up an HTTP GET request message
        post_req.version(11);
        post_req.method(http::verb::post);
        post_req.target("/api/v1/order/bulk");
        post_req.set(http::field::host, "www.bitmex.com");
        post_req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        post_req.set(http::field::accept, "*/*");
        post_req.set(http::field::content_type, "application/json");
        post_req.set(http::field::connection, "Keep-Alive");
        post_req.set("api-key", "");
        post_req.insert("Content-Length", "");
        post_req.insert("api-expires", "");
        post_req.insert("api-signature", "");
        
        // Look up the domain name
        rest_resolver.async_resolve(
            "www.bitmex.com",
            "443",
            beast::bind_front_handler(
                &BitMEX_MM::REST_on_resolve,
                shared_from_this()));
                
    }
    
};


int main(int argc, char** argv)
{
    
    net::io_context rest_ioc;
    ssl::context    rest_ctx{ssl::context::tlsv12_client};
    
    auto algo = make_shared<BitMEX_MM>(rest_ioc, rest_ctx);
    
    cout << "Running http test." << endl;
    
    algo->run_REST_service();
    
    rest_ioc.run();
    
    return 0;
}