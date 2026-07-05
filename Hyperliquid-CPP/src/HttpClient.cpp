#include "HttpClient.h"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;
using json = nlohmann::json;

std::string HttpClient::post(
    const std::string& endpoint,
    const std::string& body)
{
    net::io_context ioc;
    ssl::context ctx{ ssl::context::tlsv12_client };

    tcp::resolver resolver{ ioc };

    beast::ssl_stream<beast::tcp_stream> stream{
        ioc,
        ctx
    };

    auto const results =
        resolver.resolve(
            "api.hyperliquid.xyz",
            "443"
        );

    beast::get_lowest_layer(stream)
        .connect(results);

    SSL_set_tlsext_host_name(
        stream.native_handle(),
        "api.hyperliquid.xyz"
    );

    stream.handshake(
        ssl::stream_base::client
    );

    http::request<http::string_body> req{
        http::verb::post,
        endpoint,
        11
    };

    req.set(
        http::field::host,
        "api.hyperliquid.xyz"
    );

    req.set(
        http::field::content_type,
        "application/json"
    );

    req.body() = body;
    req.prepare_payload();

    http::write(stream, req);

    beast::flat_buffer buffer;

    http::response<http::string_body> res;

    http::read(
        stream,
        buffer,
        res
    );

    return res.body();
}