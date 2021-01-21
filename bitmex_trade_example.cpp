// g++ -std=c++17 -pthread -o http_test.out http_test.cpp -lssl -lcrypto && ./http_test.out

//Boost & Beast headers
#include <boost/bind.hpp>
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
#include <boost/log/trivial.hpp>

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

public:
        
    explicit BitMEX_MM(net::io_context& rest_ioc, ssl::context& rest_ctx, int total_tests, int expiry)
        : total_tests_ {total_tests                         }
        , rest_resolver{net::make_strand(rest_ioc)          }
        , rest_stream  {net::make_strand(rest_ioc), rest_ctx}
        , expiry_      {expiry                              }
    { }
    
private:    
    int n_tests = 1;
    int total_tests_;
    
    //REST
    tcp::resolver rest_resolver;
    beast::ssl_stream<beast::tcp_stream> rest_stream;
    beast::flat_buffer rest_buffer;

    http::request<http::string_body>  post_req;
    http::response<http::string_body> post_res;
    
    // Timing
    struct timespec start, end;
    
    //MEMBER VARIABLES
    string apiKey    = "P0Og16NeYiyN5zjrbe8C7yik";
    string apiSecret = "GrFJ9ZAJDVLa7dH41M0_S1c-UE4OSzMaI1H7qqdhOQP3WDUj";
    int    apiKeyLen = apiKey.length();
    const char* apiKeyCStr = apiKey.c_str();
    int    apiSecLen = apiSecret.length();
    const char* apiSecCStr = apiSecret.c_str();
    int    expiry_;
    
    string HMAC_SHA256_hex_POST_single(string valid_til, const std::string& order_msg)
    {
        string data = "POST/api/v1/order" + valid_til + order_msg;
        
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
            ss << std::setw(2) << std::setfill('0') << hex << static_cast<unsigned int>(out[i]);
        }
        return ss.str();
    }
    
    void REST_on_resolve(beast::error_code ec, tcp::resolver::results_type results)
    {
        beast::get_lowest_layer(rest_stream).async_connect(results, 
        [&](auto ec, auto endpoint) { REST_on_connect(ec, endpoint); });
    }

    void REST_on_connect(beast::error_code ec,
                    tcp::resolver::results_type::endpoint_type)
    {       
        rest_stream.async_handshake(ssl::stream_base::client, 
            [&](auto ec) { REST_market_order_on_handshake(ec); });
    }
    
    void REST_market_order_on_handshake(beast::error_code ec)
    {
        enum class Side { buy, sell };
        enum class OrderType { limit, market };
        const std::map<OrderType, std::string> order_type_names{
            {OrderType::limit, "Limit"}, {OrderType::market, "Market"}
        };

        const std::map<Side, std::string> side_names{
            {Side::buy, "Buy"}, {Side::sell, "Sell"}
        };

        int orderQty{2};
        Side side{Side::sell};
        OrderType type{OrderType::market};
        std::string symbol{"XBTUSD"};

        for (auto i{0}; i<total_tests_; ++i) {
            std::string order_msg =
            "{"
                "\"symbol\":\"" + symbol + "\","
                "\"ordType\":\"" + order_type_names.at(type) + "\","
                "\"side\":\"" + side_names.at(side) + "\","
                "\"orderQty\":" + std::to_string(orderQty) +
            "}";

            BOOST_LOG_TRIVIAL(info) << "Send next trade";

            int valid_til        = time(0) + expiry_;
            string valid_til_str = to_string(valid_til);
            
            post_req.set("api-expires", valid_til_str);
            post_req.set("api-signature", HMAC_SHA256_hex_POST_single(valid_til_str, order_msg));
            post_req.set("Content-Length", to_string(order_msg.length()));
            post_req.body() = order_msg;
            
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            BOOST_LOG_TRIVIAL(info) << "\nAbout to trade...  (press Enter)";
            std::cin.get();

            auto number_of_bytes_written{http::write(rest_stream, post_req)};
            BOOST_LOG_TRIVIAL(info) << "Number of bytes written to stream: " << number_of_bytes_written;

            BOOST_LOG_TRIVIAL(info) << "\nTrade written - about to read response (press Enter)";
            std::cin.get();

            auto number_of_bytes_transferred{http::read(rest_stream, rest_buffer, post_res)};
            BOOST_LOG_TRIVIAL(info) << "Number of bytes transferred from the stream: " << number_of_bytes_transferred;

            clock_gettime(CLOCK_MONOTONIC, &end);
            double time_taken;
            time_taken = (end.tv_sec  - start.tv_sec) + ((end.tv_nsec - start.tv_nsec) * 1e-9);
            BOOST_LOG_TRIVIAL(info) << "response time: " << time_taken;
            BOOST_LOG_TRIVIAL(info) << "\nNext trade...  (press Enter)";
    		std::cin.get();
        }
    }
    
public:
        
    void run_REST_service()
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
        string target{"/api/v1/order"};
        post_req.target(target);
        post_req.set(http::field::host, "www.bitmex.com");
        post_req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        post_req.set(http::field::accept, "*/*");
        post_req.set(http::field::content_type, "application/json");
        post_req.set(http::field::connection, "Keep-Alive");
        post_req.set("api-key", apiKeyCStr);
        post_req.insert("Content-Length", "");
        post_req.insert("api-expires", "");
        post_req.insert("api-signature", "");
        
        // Look up the domain name
        rest_resolver.async_resolve("www.bitmex.com", "443",
            [&](auto ec, auto results) { REST_on_resolve(ec, results); });

    }
    
};

int main(int argc, char** argv)
{
    auto usage{ [&]()->int {
        std::cout 
            << "Usage: " << argv[0] 
            << " number_of_tests"
            << " time_to_expiry"
            << std::endl;
        return 1;
    }};

    if(argc < 3) return usage();
    
    int number_of_tests{0}, time_to_expiry{0};
    try {
        number_of_tests = std::stoi(argv[1]);
        time_to_expiry = std::stoi(argv[2]);
    } catch(invalid_argument&) {
        return usage();
    }
    
    net::io_context rest_ioc;
    ssl::context    rest_ctx{ssl::context::tlsv12_client};
    
    auto algo = make_shared<BitMEX_MM>(rest_ioc, rest_ctx, number_of_tests, time_to_expiry);
    
    cout << "Running http test." << endl;
    
    algo->run_REST_service();
    
    rest_ioc.run();
    
    return 0;
}