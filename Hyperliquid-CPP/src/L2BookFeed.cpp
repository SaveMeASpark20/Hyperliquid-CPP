#include <iostream>

#include "L2BookFeed.h"

void L2BookFeed::setCallback(Callback cb)
{
	callback_ = std::move(cb);
}

void L2BookFeed::parse(const json& message)
{
	if (!message.contains("channel"))
		return;

	if (message["channel"] != "l2Book")
		return;
		
	const auto& data = message["data"];
	//std::cout << data;
	WsBook l2Book;

	const auto& bids = data["levels"][0];
	const auto& asks = data["levels"][1];

	l2Book.bids.reserve(data["levels"][0].size());
	l2Book.asks.reserve(data["levels"][1].size());

	l2Book.coin = data["coin"];
	l2Book.time = data["time"];


	for (const auto& level : bids)
	{
		l2Book.bids.push_back({
			level["px"],
			level["sz"],
			level["n"]
			});
	}

	for (const auto& level : asks)
	{
		l2Book.asks.push_back({
			level["px"],
			level["sz"],
			level["n"]
			});
	}

	
	if (callback_) callback_(l2Book);
}