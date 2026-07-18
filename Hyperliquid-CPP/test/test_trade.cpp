#include "WSConnect.h"
#include <iostream>
#include <iomanip>
#include <curl/curl.h>

#include <secp256k1.h>
#include <boost/beast/core.hpp>
#include <nlohmann/json.hpp>

#include <array>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <fstream>

//#include "HyperliquidSigner.h" 
#include "HyperliquidClient.h" 
#include "HyperliquidInfo.h" 
#define MODE_TEST 1 


#ifdef _WIN32
#pragma comment(lib, "libcurl.lib")
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
        // =====================================================
        // 1. Create signer
        // =====================================================

        HyperliquidClient client;
        //
        //Order order;
        /*OrderWire order;

        order.asset = 0;
        order.is_buy = true;
        order.px = "64876";
        order.sz = "10.27977";
        order.reduce_only = false;
        order.tif = "Ioc";*/


        //client.placeOrder(order);


        //HyperliquidSigner signer;
        UpdateLeverageAction leverage;

        leverage.type = "updateLeverage";
        leverage.asset = 0;
        leverage.is_cross = true;
        leverage.leverage = 3;
        client.updateLeverage(leverage);


        //HyperliquidInfo info;

        //std::string result = info.spotUserState("0xeD0Bb03089Ae4ff5A97C84642F17654bbf839cC9");

        //std::cout << "result: " << result << "/n";
        //signer.signL1Action(leverage, "", true);

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