#ifndef	DISPATCHER_H
#define DISPATCHER_H

#include <iostream>
#include <nlohmann/json.hpp>

#include "TradeFeed.h"
#include "BBOFeed.h"
#include "L2BookFeed.h"

using json = nlohmann::json;

class Dispatcher
{
public:
	void dispatch(const json& message);

	TradeFeed& tradeFeed()
	{
		return tradeFeed_;
	};

	BBOFeed& bboFeed()
	{
		return bboFeed_;
	};

	L2BookFeed& l2BookFeed()
	{
		return l2BookFeed_;
	};

private:
	TradeFeed tradeFeed_;
	BBOFeed bboFeed_;
	L2BookFeed l2BookFeed_;


};

#endif

