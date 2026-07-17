#ifndef WSCONNECT_H
#define WSCONNECT_H



#include <string>
#include <string_view>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>

#include "Dispatcher.h"

namespace beast = boost::beast;
//namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;


//class TradeFeed;

class Dispatcher;

class WSConnect
{

public:
	WSConnect();
	void connect();

	Dispatcher& dispatcher()
	{
		return dispatcher_;
	};

	void subscribeBBO(std::string_view coin);
	void subscribeTrades(std::string_view coin);
	void subscribeL2Book(std::string_view coin);
private:
	//TradeFeed* tradeFeed_ = nullptr;
	net::io_context ioc_;
	ssl::context ssl_;
	websocket::stream<
		beast::ssl_stream<tcp::socket>
	> ws_;
	beast::flat_buffer buffer_;
	Dispatcher dispatcher_;
};

#endif