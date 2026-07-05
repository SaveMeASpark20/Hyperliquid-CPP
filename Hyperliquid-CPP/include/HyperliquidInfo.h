#ifndef HYPERLIQUIDINFO_H
#define HYPERLIQUIDINFO_H

#include "HttpClient.h"
#include <string>

class HyperliquidInfo
{
public:
	std::string spotUserState(std::string address);

private:
	HttpClient httpClient_;
};


#endif