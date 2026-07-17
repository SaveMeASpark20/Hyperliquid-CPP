#ifndef BBOFEED_H
#define BBOFEED_H

#include <iostream>
#include <nlohmann/json.hpp>
#include "WSTypes.h"

#include <functional>

using json = nlohmann::json;

class BBOFeed
{
public:
	using Callback = std::function<void(const WsBbo&)>;

	void setCallback(Callback cb);

	void parse(const json& message);
private:
	Callback callback_;
};

#endif
