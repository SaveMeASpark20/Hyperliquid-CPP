#ifndef TRADEFEED_H
#define TRADEFEED_H

#include <functional>
#include <nlohmann/json.hpp>

#include "WSTypes.h"

using json = nlohmann::json;

class TradeFeed
{

public:
	using Callback = std::function<void(const Trade&)>;

	void  setCallback(Callback cb);

	void parse(const json& message);

private:
	Callback callback_;
};

#endif