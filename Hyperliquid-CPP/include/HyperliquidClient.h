#ifndef HYPERLIQUIDCLIENT_H
#define HYPERLIQUIDCLIENT_H

#include <string>
#include "HyperliquidSigner.h"
#include "HttpClient.h"
#include "HyperliquidTypes.h"


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

