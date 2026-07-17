#include <iostream>

#include "TradeFeed.h"

void TradeFeed::setCallback(Callback cb)
{
	callback_ = std::move(cb);
}

void TradeFeed::parse(const json& message)
{
	if (!message.contains("channel"))
		return;

	if (message["channel"] != "trades")
		return;

	const auto& trades = message["data"];

	for (const auto& t : trades)
	{
		Trade trade;

		trade.coin = t["coin"];

		trade.side = t["side"] == "B";

		trade.px = t["px"];

		trade.sz = t["sz"];

		trade.hash = t["hash"];

		trade.time = t["time"];

		trade.tid = t["tid"];

		trade.buyer = t["users"][0];

		trade.seller = t["users"][1];
		
		if(callback_) callback_(trade);
	}
}
