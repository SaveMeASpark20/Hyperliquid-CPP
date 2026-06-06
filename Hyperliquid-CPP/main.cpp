#include "WSConnect.h"
#include "HyperliquidSigner.h" 
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



using json = nlohmann::json;

struct Config {
    std::string private_key;
    std::string wallet_address;
};

Config loadConfig() {
    std::ifstream file("config.json");

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config.json");
    }

    json j;
    file >> j;

    return {
        j["private_key"],
        j["wallet_address"]
    };
}

int main()
{
    try
    {
        // =====================================================
        // 1. Create signer
        // =====================================================

        HyperliquidSigner signer;
        Config cfg = loadConfig();

        // =====================================================
        // 2. Wallet
        // =====================================================

        LocalAccount wallet;

        wallet.address = cfg.wallet_address;

        wallet.private_key = cfg.private_key;

        // =====================================================
        // 3. Create order
        // =====================================================

        OrderWire order;

        order.asset = 0;
        order.is_buy = true;
        order.px = "64445";
        order.sz = "11.58889";
        order.reduce_only = false;
        order.tif = "Ioc";

        OrderAction action;
        action.orders.push_back(order);
        action.grouping = "na";

        // =====================================================
        // 5. Nonce (timestamp ms)
        // =====================================================

        uint64_t nonce = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();


        //std::cout << "ACTION" << action;
        // =====================================================
        // 6. Sign
        // =====================================================

        SignedL1Action signed_action =
            signer.signL1Action(
                wallet,
                action,
                "",         // vault address
                nonce,
                true       // false = testnet
            );

        // =====================================================
        // 7. Print order JSON
        // =====================================================

        /*std::cout
            << "ORDER JSON:"
            << std::endl;

        std::cout
            << action.serialize()
            << std::endl;*/

        // =====================================================
        // 8. Print signature
        // =====================================================

        std::cout
            << "R: "
            << bytesToHex(
                std::vector<uint8_t>(
                    signed_action.signature.r.begin(),
                    signed_action.signature.r.end()
                )
            )
            << std::endl;

        std::cout
            << "S: "
            << bytesToHex(
                std::vector<uint8_t>(
                    signed_action.signature.s.begin(),
                    signed_action.signature.s.end()
                )
            )
            << std::endl;

        std::cout
            << "V: "
            << signed_action.signature.v
            << std::endl;

        // =====================================================
        // 9. Print payload
        // =====================================================

        std::cout
            << "SOURCE: "
            << signed_action.payload.message.source
            << std::endl;

        std::cout
            << "CONNECTION ID: "
            << bytesToHex(signed_action.payload.message.connectionId)
            << std::endl;

        // =====================================================
        // 10. Success
        // =====================================================

        std::cout
            << "SIGN SUCCESS"
            << std::endl;
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