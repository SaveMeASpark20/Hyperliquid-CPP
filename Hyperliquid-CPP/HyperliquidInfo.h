#ifndef HYPERLIQUIDINFO_H
#define HYPERLIQUIDINFO_H

#include "HttpClient.h"

#include <nlohmann/json.hpp>
#include <string>


using json = nlohmann::json;


class HyperliquidInfo
{
public:
	std::string spotUserState(std::string address);

private:
	HttpClient httpClient_;
};


#endif