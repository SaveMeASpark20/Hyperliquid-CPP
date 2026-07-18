#include "WSConnect.h"
#include <iostream>
 
#include "HyperliquidClient.h" 
#include "HyperliquidInfo.h" 

#include "WSConnect.h"

#define MODE_TEST 1 


#ifdef _WIN32
    #pragma comment(lib, "libssl.lib")
    #pragma comment(lib, "libcrypto.lib")
    #pragma comment(lib, "crypt32.lib")
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "cryptopp.lib") 
    #pragma comment(lib, "Normaliz.lib") 
    #pragma comment(lib, "Wldap32.lib") 
    #pragma comment(lib, "advapi32.lib") 
#endif

using json = nlohmann::json;

void print_hex(const std::vector<uint8_t>& vec) {
    for (uint8_t b : vec) {
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)b;
    }
    std::cout << std::endl;
}


int main()
{
    try
    {
        //WSConnect ws;
        //ws.connect();

        //ws.dispatcher().tradeFeed().setCallback(
        //    [](const Trade& trade)
        //    {
                //std::cout
                //    << trade.coin
                //    << " "
                //    << trade.px
                //    << " "
                //    << trade.sz
                //    << "\n";
            //});

        //ws.subscribeTrades("BTC");

        //ws.dispatcher().bboFeed().setCallback(
        //    [](const WsBbo& bbo)
        //    {
            //    std::cout
            //        << bbo.coin
            //        << " "
            //        << bbo.time
            //        << " "
            //        << "\n"; 
            //        if (bbo.bbo1)
            //        {
            //            std::cout << bbo.bbo1->px
            //                << " "
            //                << bbo.bbo1->sz
            //                << " "
            //                << bbo.bbo1->n
            //                << "\n";
            //        }

            //        if (bbo.bbo2)
            //        {
            //            std::cout << bbo.bbo2->px
            //            << " "
            //            << bbo.bbo2->sz
            //            << " "
            //            << bbo.bbo2->n
            //            << "\n";
            //        }
            //});

        //ws.subscribeBBO("BTC");

        //ws.dispatcher().l2BookFeed().setCallback(
        //    [](const WsBook& l2Book)
        //    {
        //        //std::cout << l2Book.coin << "\n";
        //        for (const auto& level : l2Book.bids)
        //        {
        //            std::cout
        //                << "bids: " << "\n"
        //                << level.px << " "
        //                << level.sz << " "
        //                << level.n << "\n";
        //        }

        //        for (const auto& level : l2Book.asks)
        //        {
        //            std::cout
        //                << "asks: " << "\n"
        //                << level.px << " "
        //                << level.sz << " "
        //                << level.n << "\n";
        //        }
        //    });

        //ws.subscribeL2Book("BTC");
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "ERROR: "
            << ex.what()
            << std::endl;
    }

    return 0;
}