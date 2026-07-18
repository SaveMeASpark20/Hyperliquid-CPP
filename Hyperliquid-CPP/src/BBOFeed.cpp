
#include "BBOFeed.h"

void BBOFeed::setCallback(
	Callback cb)
{
	callback_ = std::move(cb);
}

void BBOFeed::parse(const json& message)
{
	if (!message.contains("channel"))
		return;

	if (message["channel"] != "bbo")
		return;

	const auto& data = message["data"];
	WsBbo bbo;
	
	std::cout << "bbo: " << data << "\n";
	bbo.coin = data["coin"];
	bbo.time = data["time"];
		
	if (!data["bbo"][0].is_null()) 
		bbo.bbo1 = WsLevel{
			data["bbo"][0]["px"],
			data["bbo"][0]["sz"],
			data["bbo"][0]["n"]
		};
	if (!data["bbo"][1].is_null()) 
		bbo.bbo1 = WsLevel{
			data["bbo"][1]["px"],
			data["bbo"][1]["sz"],
			data["bbo"][1]["n"]
		};
	if (callback_) callback_(bbo);
	
}