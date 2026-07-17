#include "WSConnect.h"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

//#include "OrderBook.h"


namespace beast = boost::beast;
//namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;
using json = nlohmann::json;


WSConnect::WSConnect()
	:ssl_(ssl::context::tlsv12_client),
	ws_(ioc_, ssl_)
{
}

void WSConnect::connect()
{
	try
	{
		const std::string host = "api.hyperliquid.xyz";
		const std::string port = "443";

		tcp::resolver resolver{ ioc_ };
		

		auto const results = resolver.resolve(host, port);

		// TCP Connect: Establishes a reliable "pipe" using the 3-Way Handshake. 
// Takes 1.5 to 3 round trips. Guarantees no data is lost or out of order.
		net::connect(beast::get_lowest_layer(ws_), results.begin(), results.end());

		if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host.c_str()))
		{
			throw beast::system_error(
				beast::error_code(
					static_cast<int>(::ERR_get_error()),
					net::error::get_ssl_category()),
				"Failed to set SNI Hostname");
		}
		// TLS Handshake: The "Secret Whisper." Swaps encryption keys so the pipe is secure.
		// Takes 2 round trips in TLS 1.2.
		ws_.next_layer().handshake(ssl::stream_base::client);
		// WebSocket Handshake: Upgrades the "pipe" from a standard website request 
		// to a permanent, two-way live stream.
		ws_.handshake(host, "/ws");

		std::cout << "Connected to Hyperliquid! \n";	
	}
	catch (std::exception const& e) {
		std::cerr << "Error: " << e.what() << "\n";
	}
};

void WSConnect::subscribeBBO(std::string_view coin) {
	json subscribe_msg = {
			{"method", "subscribe"},
			{"subscription", {
				{"type", "bbo"},
				{"coin", coin}
			}}
	};

	ws_.write(net::buffer(subscribe_msg.dump()));

	for (;;) {
		buffer_.consume(buffer_.size());
		ws_.read(buffer_);
		auto response = json::parse(beast::buffers_to_string(buffer_.data()));

		dispatcher_.dispatch(response);
	}
}

void WSConnect::subscribeTrades(std::string_view coin) {
	json subscribe_msg = {
			{"method", "subscribe"},
			{"subscription", {
				{"type", "trades"},
				{"coin", coin}
			}}
	};

	ws_.write(net::buffer(subscribe_msg.dump()));

	for (;;) {
		buffer_.consume(buffer_.size());
		ws_.read(buffer_);
		auto response = json::parse(beast::buffers_to_string(buffer_.data()));

		dispatcher_.dispatch(response);
	}
}

void WSConnect::subscribeL2Book(std::string_view coin) {
	json subscribe_msg = {
			{"method", "subscribe"},
			{"subscription", {
				{"type", "l2Book"},
				{"coin", coin}
			}}
	};

	ws_.write(net::buffer(subscribe_msg.dump()));

	for (;;) {
		buffer_.consume(buffer_.size());
		ws_.read(buffer_);
		auto response = json::parse(beast::buffers_to_string(buffer_.data()));
		//std::cout << response << '\n';
		dispatcher_.dispatch(response);
	}
}