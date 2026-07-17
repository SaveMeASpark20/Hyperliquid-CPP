#include "Dispatcher.h"
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


void Dispatcher::dispatch(const json& message)
{
	//std::cout << "message" << message << '\n';
	if (!message.contains("channel"))
		return;

	if (message["channel"] == "trades")
	{
		tradeFeed_.parse(message);
	}

	if (message["channel"] == "bbo")
	{
		bboFeed_.parse(message);
	}

	if (message["channel"] == "l2Book")
	{
		l2BookFeed_.parse(message);
	}
}