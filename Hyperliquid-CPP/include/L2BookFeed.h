#ifndef L2BOOKFEED_H
#define L2BOOKFEED_H

#include "WSTypes.h"
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class L2BookFeed
{
public:
	using Callback = std::function<void(const WsBook)>;

	void setCallback(Callback cb);

	void parse(const json& message);

private:
	Callback callback_;

};

#endif