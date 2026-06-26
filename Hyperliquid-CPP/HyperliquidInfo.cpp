#include "HyperliquidInfo.h"


std::string HyperliquidInfo::spotUserState(std::string address)
{
	json payload = { 
		{"type", "spotClearinghouseState"},
		{"user", address}
	};

	std::string response = httpClient_.post("/info", payload.dump());
	return response;
}