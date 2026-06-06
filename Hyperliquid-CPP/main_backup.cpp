//#include "WSConnect.h"
//#include "HyperliquidSigner.h" 
//#include <iostream>
//#include <iomanip>
//#include <curl/curl.h>
//
//#include <secp256k1.h>
//#include <boost/beast/core.hpp>
//#include <nlohmann/json.hpp>
//
//
//#define MODE_TEST 1 
//
//#include <iomanip>
//
//
//#ifdef _WIN32
//#pragma comment(lib, "libcurl.lib")
//#pragma comment(lib, "libssl.lib")
//#pragma comment(lib, "libcrypto.lib")
//#pragma comment(lib, "crypt32.lib")
//#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "cryptopp.lib") 
//#pragma comment(lib, "Normaliz.lib") 
//#pragma comment(lib, "Wldap32.lib") 
//#pragma comment(lib, "advapi32.lib") 
//#endif
//
//
//using json = nlohmann::json;
//
//// Helper to convert bytes to hex string (if not already in your Signer class)
//std::string bytesToHex(const std::vector<uint8_t>& bytes) {
//    std::stringstream ss;
//    ss << std::hex << std::setfill('0');
//    for (uint8_t b : bytes) ss << std::setw(2) << (int)b;
//    return ss.str();
//}
//
//void printJsonSignature(const std::vector<uint8_t>& sig) {
//    if (sig.size() < 65) return;
//
//    std::vector<uint8_t> r(sig.begin(), sig.begin() + 32);
//    std::vector<uint8_t> s(sig.begin() + 32, sig.begin() + 64);
//    int v = sig[64];
//
//    std::cout << "\n--- COPY THIS INTO YOUR JSON PAYLOAD ---" << std::endl;
//    std::cout << "\"signature\": {" << std::endl;
//    std::cout << "  \"r\": \"0x" << bytesToHex(r) << "\"," << std::endl;
//    std::cout << "  \"s\": \"0x" << bytesToHex(s) << "\"," << std::endl;
//    std::cout << "  \"v\": " << v << std::endl;
//    std::cout << "}" << std::endl;
//}
//
//void sendOrderToExchange(const std::string& jsonPayload, bool isTestnet) {
//    CURL* curl = curl_easy_init();
//    if (curl) {
//        const char* url = isTestnet
//            ? "https://api.hyperliquid-testnet.xyz/exchange"
//            : "https://api.hyperliquid.xyz";
//
//        struct curl_slist* headers = NULL;
//        headers = curl_slist_append(headers, "Content-Type: application/json");
//
//        curl_easy_setopt(curl, CURLOPT_URL, url);
//        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//
//        CURLcode res = curl_easy_perform(curl);
//        if (res != CURLE_OK) std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
//
//        curl_easy_cleanup(curl);
//    }
//}
//
//
//int main_backup() {
//    HyperliquidSigner signer;
//
//    // 1. Setup Order (Example: Buy BTC at $50,000)
//    HyperliquidOrder order;
//    order.asset = 0;
//    order.isBuy = true;
//    order.limitPrice = 5000000000; // $50,000 * 10^5
//    order.size = 1000000;          // 0.01 BTC * 10^8
//    order.nonce = signer.getCurrentNonce(); // Get REAL time
//    order.reduceOnly = false;
//
//    bool isTestnet = true;
//    std::string myPrivKeyHex = "0x462411ebe54bc58ebdfa8c75ccb6e33ec2edcfb51deba9d6a1c5f1e077517f1e"; // Use a Testnet key!
//
//
//    try {
//        // 1. Create the dummy order hash (connectionId) used in Python
//        // connection_id_hex = "0x7465737400000000000000000000000000000000000000000000000000000000"
//        std::string testConnectionIdHex = "7465737400000000000000000000000000000000000000000000000000000000";
//        std::vector<uint8_t> orderHash = signer.hexToBytes(testConnectionIdHex);
//
//        // 2. Set nonce (Note: In your Python script, the nonce is implicit in the Domain, but HL trading uses it in the action hash)
//        // For this specific test, we check if getFinalActionHash produces 0x49105470...
//        bool isTestnet = true; // Should match "HyperliquidChainTestnet"
//
//        auto finalHash = signer.getFinalActionHash(orderHash, order.nonce, isTestnet);
//
//        std::cout << "DEBUG COMPARISON:" << std::endl;
//        std::cout << "PYTHON EXPECTED: 0x491054704ef14e911516110981d876204693d337770ba1838b3b736027e17377" << std::endl;
//        std::cout << "C++ RESULT:      0x" << signer.toHex(finalHash) << std::endl;
//
//        if (signer.toHex(finalHash) == "491054704ef14e911516110981d876204693d337770ba1838b3b736027e17377") {
//            std::cout << "SUCCESS: EIP-712 Packing is correct!" << std::endl;
//        }
//        else {
//            std::cout << "FAILED: Discrepancy in Domain Separator or Agent TypeHash." << std::endl;
//        }
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Error: " << e.what() << std::endl;
//    }
//
//    return 0;
//}

// Helper function to print hex
//void printHex(const std::string& label, const std::vector<uint8_t>& data) {
//	std::cout << label << ": 0x";
//	for (auto b : data) {
//		std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)b;
//	}
//	std::cout << std::dec << std::endl;
//}

//int main()
//{
//#if MODE_TEST == 1
//	try
//	{
//		// 1. Convert "hello" to bytes
//		std::string input = "hello";
//		std::vector<uint8_t> inputBytes(input.begin(), input.end());
//
//		// 2. Hash it
//		std::vector<uint8_t> hash = HyperliquidSigner::keccak256(inputBytes);
//
//		// 3. Print it (it should start with 1c8a...)
//		std::cout << "Keccak('hello'): ";
//		for (auto b : hash) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
//		std::cout << std::dec << std::endl;
//
//	}
//	catch (const std::exception& e) {
//		std::cerr << "Test failed: " << e.what() << std::endl;
//	}
//
//#else
//	// --- RUNNING THE LIVE WEBSOCKET ---
//	WSConnect client;
//	client.connectWS("BTC");
//#endif
//
//	return 0;
//}



/*
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <cryptopp/keccak.h>

#pragma comment(lib, "libsecp256k1.lib")

struct HyperliquidOrder {
    uint32_t asset;
    bool isBuy;
    uint64_t limitPrice;
    uint64_t size;
    uint64_t nonce;
};



    // Standard Hyperliquid Mainnet Domain Separator Hash
    std::vector<uint8_t> getDomainHash() {
        // Pre-computed hash for name="HyperliquidExchange", chainId=42161
        // From: keccak256(abi.encode(TYPE_HASH, hash("HyperliquidExchange"), hash("1"), 42161, 0x0...))
        const char* hex = "8b73c3c69bb8fe3d512ecc4cf759cc79239f7b179b0ffacaa9a75d522b39400f";
        return hexToBytes(hex);
    }

    // EIP-712 Final Hash: \x19\x01 || DomainHash || OrderHash
    std::vector<uint8_t> getFinalHash(const std::vector<uint8_t>& orderHash) {
        std::vector<uint8_t> finalData;
        finalData.push_back(0x19);
        finalData.push_back(0x01);
        auto domain = getDomainHash();
        finalData.insert(finalData.end(), domain.begin(), domain.end());
        finalData.insert(finalData.end(), orderHash.begin(), orderHash.end());
        return keccak256(finalData);
    }
};

int main() {
    HyperliquidSigner signer;

    // 1. Example Setup (Use a testnet private key or dummy)
    std::string dummyPrivKeyHex = "0000000000000000000000000000000000000000000000000000000000000001";
    auto privKey = signer.hexToBytes(dummyPrivKeyHex);

    // 2. Dummy Order Hash (In production, replace with your hashOrder logic)
    std::vector<uint8_t> dummyOrderHash(32, 0xAB);

    // 3. Generate Final EIP-712 Hash
    auto finalHash = signer.getFinalHash(dummyOrderHash);

    // 4. Sign
    auto signature = signer.sign(finalHash, privKey);

    // 5. Print Result
    std::cout << "Final EIP-712 Hash: ";
    for(auto b : finalHash) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << "\nSignature (r,s,v): ";
    for(auto b : signature) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << std::endl;

    return 0;
}
*/