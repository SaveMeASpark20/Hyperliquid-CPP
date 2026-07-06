#include "HyperliquidClient.h"

    
void HyperliquidClient::placeOrder(const OrderWire& order)
{
    
    OrderAction orderAction;
    orderAction.orders.push_back(order);
    orderAction.grouping = "na";

    // Sign payload    
    SignedL1Action signed_action =
        signer_.signL1Action(
            orderAction,
            "",         
            true       
        );

    json payload = signer_.toJson(signed_action);

    std::string response =
        httpClient_.post(
            "/exchange",
            payload.dump()
        );
    
    std::cout
        << payload.dump(4)
        << std::endl;

    
    std::cout
        << "RESPONSE:\n"
        << response
        << std::endl;
    
    

   
}

void HyperliquidClient::updateLeverage(const UpdateLeverageAction& leverage)
{   
    sendAction(leverage);
}


void HyperliquidClient::sendAction(const Action& action)
{
    
    // Sign payload    
    SignedL1Action signed_action =
        signer_.signL1Action(
            action,
            "",
            true
        );

    json payload = signer_.toJson(signed_action);

    std::string response =
        httpClient_.post(
            "/exchange",
            payload.dump()
        );

    std::cout
        << payload.dump(4)
        << std::endl;


    std::cout
        << "RESPONSE:\n"
        << response
        << std::endl;


}

