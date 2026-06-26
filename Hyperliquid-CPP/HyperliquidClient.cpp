#include "HyperliquidClient.h"

    
void HyperliquidClient::placeOrder(const OrderWire& order)
{
    // Build action
    /*OrderWire orderWire;
    
    orderWire.asset = order.asset;
    orderWire.is_buy = order.isBuy;
    orderWire.sz = order.size;
    orderWire.px = order.price;
    orderWire.reduce_only = order.reduceOnly;
    orderWire.tif = order.tif;*/

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
    
    

    //std::cout
    //    << "R: "
    //    << bytesToHex(
    //        std::vector<uint8_t>(
    //            signed_action.signature.r.begin(),
    //            signed_action.signature.r.end()
    //        )
    //    )
    //    << std::endl;

    //std::cout
    //    << "S: "
    //    << bytesToHex(
    //        std::vector<uint8_t>(
    //            signed_action.signature.s.begin(),
    //            signed_action.signature.s.end()
    //        )
    //    )
    //    << std::endl;

    //std::cout
    //    << "V: "
    //    << signed_action.signature.v
    //    << std::endl;

    //// =====================================================
    //// 9. Print payload
    //// =====================================================

    //std::cout
    //    << "SOURCE: "
    //    << signed_action.payload.message.source
    //    << std::endl;

    //std::cout
    //    << "CONNECTION ID: "
    //    << bytesToHex(signed_action.payload.message.connectionId)
    //    << std::endl;

    //// =====================================================
    //// 10. Success
    //// =====================================================

    //std::cout
    //    << "SIGN SUCCESS"
    //    << std::endl;

    //// Send POST request
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



    //std::cout
    //    << "R: "
    //    << bytesToHex(
    //        std::vector<uint8_t>(
    //            signed_action.signature.r.begin(),
    //            signed_action.signature.r.end()
    //        )
    //    )
    //    << std::endl;

    //std::cout
    //    << "S: "
    //    << bytesToHex(
    //        std::vector<uint8_t>(
    //            signed_action.signature.s.begin(),
    //            signed_action.signature.s.end()
    //        )
    //    )
    //    << std::endl;

    //std::cout
    //    << "V: "
    //    << signed_action.signature.v
    //    << std::endl;

    //// =====================================================
    //// 9. Print payload
    //// =====================================================

    //std::cout
    //    << "SOURCE: "
    //    << signed_action.payload.message.source
    //    << std::endl;

    //std::cout
    //    << "CONNECTION ID: "
    //    << bytesToHex(signed_action.payload.message.connectionId)
    //    << std::endl;

    //// =====================================================
    //// 10. Success
    //// =====================================================

    //std::cout
    //    << "SIGN SUCCESS"
    //    << std::endl;

    //// Send POST request
}

