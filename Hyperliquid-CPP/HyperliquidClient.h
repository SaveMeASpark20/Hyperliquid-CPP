#ifndef HYPERLIQUIDCLIENT_H
#define HYPERLIQUIDCLIENT_H

#include <string>
#include <nlohmann/json.hpp>

#include "HyperliquidSigner.h"
#include "HttpClient.h"

using json = nlohmann::json;

struct Order
{
    uint32_t asset;
    bool isBuy;
    std::string size;
    std::string price;
    bool reduceOnly;
    std::string tif; // "Gtc", "Ioc", "Alo"
};

class HyperliquidClient
{
	public:
		void placeOrder(const Order& order);
	private:
        HyperliquidSigner signer_;
        HttpClient httpClient_;

	
};


#endif

