#ifndef HYPERLIQUIDCLIENT_H
#define HYPERLIQUIDCLIENT_H

#include <string>
#include "HyperliquidSigner.h"
#include "HttpClient.h"
#include "HyperliquidTypes.h"


//struct Order
//{
//    uint32_t asset;
//    bool isBuy;
//    std::string size;
//    std::string price;
//    bool reduceOnly;
//    std::string tif; // "Gtc", "Ioc", "Alo"
//};

class HyperliquidClient
{
	public:
		void placeOrder(const OrderWire& order);
		void updateLeverage(const UpdateLeverageAction& leverage);
	private:
		void sendAction(const Action& action);
        HyperliquidSigner signer_;
        HttpClient httpClient_;

	
};


#endif

