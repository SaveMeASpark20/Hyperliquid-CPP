#include "HyperliquidSigner.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <msgpack.hpp>          
#include <cryptopp/keccak.h>
#include <iomanip> 
#include <nlohmann/json.hpp>
#include <algorithm>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <stdexcept>
#include <fstream>
#include <string>
#include <variant>

using json = nlohmann::json;




Config HyperliquidSigner::loadConfig() {
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

json HyperliquidSigner::actionToJson(const OrderAction& action)
{
    json orders = json::array();

    for (const auto& o : action.orders)
    {
        json order;

        order["a"] = o.asset;
        order["b"] = o.is_buy;
        order["p"] = o.px;
        order["s"] = o.sz;
        order["r"] = o.reduce_only;
        order["t"] = {
            {
                "limit",
                {
                    {"tif", o.tif}
                }
            }
        };

        if (!o.cloid.empty())
            order["c"] = o.cloid;

        orders.push_back(std::move(order));
        
    }
    
    return {
        {"type", "order"},
        {"orders", orders},
        {"grouping", action.grouping}
    };
};

json HyperliquidSigner::actionToJson(const UpdateLeverageAction& action)
{
    return {
        {"type", "updateLeverage"},
        {"asset", action.asset},
        {"isCross", action.is_cross},
        {"leverage", action.leverage}
    };
}


json HyperliquidSigner::toJson(const SignedL1Action& signedAction)
{
    json payload;


    payload["action"] =
        std::visit(
            [this](const auto& action)
            {
                return actionToJson(action);
            },
            signedAction.action
        );

    payload["nonce"] = signedAction.nonce;

    payload["signature"] = {
        {
            "r",
            "0x" + bytesToHex(
                std::vector<uint8_t>(
                    signedAction.signature.r.begin(),
                    signedAction.signature.r.end()
                )
            )
        },
        {
            "s",
            "0x" + bytesToHex(
                std::vector<uint8_t>(
                    signedAction.signature.s.begin(),
                    signedAction.signature.s.end()
                )
            )
        },
        {"v", signedAction.signature.v}
    };

    payload["vaultAddress"] =
        signedAction.vaultAddress.has_value()
        ? json(*signedAction.vaultAddress)
        : nullptr;

    payload["expiresAfter"] =
        signedAction.expiresAfter.has_value()
        ? json(*signedAction.expiresAfter)
        : nullptr;

    return payload;
}




std::vector<uint8_t> HyperliquidSigner::keccak256(const std::vector<uint8_t>& input) 
{
    using namespace CryptoPP;
    std::vector<uint8_t> digest(32);

    // CRITICAL: We use Keccak_256 class but ensure no SHA-3 padding
    CryptoPP::Keccak_256 hash;
    hash.Update(input.data(), input.size());
    hash.Final(digest.data());

    return digest;
}

std::vector<uint8_t> HyperliquidSigner::hexToBytes(std::string hex)
{
    if (hex.compare(0, 2, "0x") == 0) hex = hex.substr(2);

    std::vector<uint8_t> bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2)
    {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }

    return bytes;
}

std::vector<uint8_t>
HyperliquidSigner::packAction(
    const Action& action
)
{
    return std::visit(
        [this](auto&& a)
        {
            return packActionImpl(a);
        },
        action
    );
}

std::vector<uint8_t>
HyperliquidSigner::packActionImpl(
    const OrderAction& action
)
{
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> pk(&sbuf);

    //
    // Python insertion order:
    // type, orders, grouping, builder
    //
    const uint32_t mapSize =
        action.builder ? 4 : 3;

    pk.pack_map(mapSize);

    //
    // "type":"order"
    //
    pk.pack(std::string("type"));
    pk.pack(std::string("order"));

    //
    // "orders":[...]
    //
    pk.pack(std::string("orders"));
    pk.pack_array(action.orders.size());

    for (const auto& order : action.orders)
    {
        //
        // Python insertion order:
        // a,b,p,s,r,t,c
        //
        const uint32_t orderMapSize =
            order.cloid.empty() ? 6 : 7;

        pk.pack_map(orderMapSize);

        pk.pack(std::string("a"));
        pk.pack(order.asset);

        pk.pack(std::string("b"));
        pk.pack(order.is_buy);

        pk.pack(std::string("p"));
        pk.pack(order.px);

        pk.pack(std::string("s"));
        pk.pack(order.sz);

        pk.pack(std::string("r"));
        pk.pack(order.reduce_only);

        //
        // "t":{"limit":{"tif":"Gtc"}}
        //
        pk.pack(std::string("t"));

        pk.pack_map(1);               // {limit:...}

        pk.pack(std::string("limit"));

        pk.pack_map(1);               // {tif:...}

        pk.pack(std::string("tif"));
        pk.pack(order.tif);

        //
        // optional cloid
        //
        if (!order.cloid.empty())
        {
            pk.pack(std::string("c"));
            pk.pack(order.cloid);
        }
    }

    //
    // "grouping":"na"
    //
    pk.pack(std::string("grouping"));
    pk.pack(action.grouping);

    //
    // optional builder
    //
    if (action.builder)
    {
        pk.pack(std::string("builder"));

        pk.pack_map(2);

        pk.pack(std::string("b"));
        pk.pack(action.builder->b);

        pk.pack(std::string("f"));
        pk.pack(action.builder->f);
    }


    std::vector<uint8_t> packed(
        reinterpret_cast<const uint8_t*>(sbuf.data()),
        reinterpret_cast<const uint8_t*>(sbuf.data()) + sbuf.size()
    );

    std::cout << "MessagePack (" << packed.size() << " bytes): ";

    for (uint8_t b : packed)
    {
        printf("%02x", b);
    }

    std::cout << std::endl;

    return packed;
}

std::vector<uint8_t>
HyperliquidSigner::packActionImpl(
    const UpdateLeverageAction& action
)
{
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> pk(&sbuf);

    pk.pack_map(4);

    pk.pack("type");
    pk.pack("updateLeverage");

    pk.pack("asset");
    pk.pack(action.asset);

    pk.pack("isCross");
    pk.pack(action.is_cross);

    pk.pack("leverage");
    pk.pack(action.leverage);

    return {
        reinterpret_cast<const uint8_t*>(sbuf.data()),
        reinterpret_cast<const uint8_t*>(sbuf.data()) + sbuf.size()
    };
}


std::vector<uint8_t>
HyperliquidSigner::getActionHash(
    const Action& action,
    uint64_t nonce,
    const std::string& vaultAddress,
    std::optional<uint64_t> expiresAfter = std::nullopt
)
{
    std::vector<uint8_t> data = packAction(action);

   /* std::visit([](const auto& value) {
        std::cout << value << std::endl;
        }, action);*/

    //
    // nonce.to_bytes(8, "big")
    //
    for (int i = 7; i >= 0; --i)
    {
        data.push_back(
            static_cast<uint8_t>((nonce >> (i * 8)) & 0xFF)
        );
    }

    //
    // vault address
    //
    if (vaultAddress.empty())
    {
        data.push_back(0x00);
    }
    else
    {
        data.push_back(0x01);

        std::vector<uint8_t> addrBytes =
            hexToBytes(vaultAddress);

        data.insert(
            data.end(),
            addrBytes.begin(),
            addrBytes.end()
        );
    }

    //
    // expires_after
    //
    if (expiresAfter.has_value())
    {
        data.push_back(0x00);

        uint64_t exp = *expiresAfter;

        for (int i = 7; i >= 0; --i)
        {
            data.push_back(
                static_cast<uint8_t>((exp >> (i * 8)) & 0xFF)
            );
        }
    }

    std::cout << "HASH INPUT HEX: ";

    for (uint8_t b : data)
    {
        printf("%02x", b);
    }

    printf("\n");

    auto hash = keccak256(data);

    std::cout << "ACTION HASH: ";

    for (uint8_t b : hash)
    {
        printf("%02x", b);
    }

    printf("\n");

    std::cout << "\nDATA SIZE: "
        << data.size()
        << std::endl;

    std::cout << "NONCE: "
        << nonce
        << std::endl;

    std::cout << "LAST 9 BYTES: ";

    for (size_t i = data.size() - 9; i < data.size(); ++i)
    {
        printf("%02x", data[i]);
    }

    printf("\n");

    return hash;
}

AgentMessage HyperliquidSigner::getInfo(bool is_mainnet, std::vector<uint8_t> hash)
{
    return { is_mainnet ? "a" : "b", hash };
}


L1Payload HyperliquidSigner::payLoadL1(AgentMessage phantom_agent) {
    return {
        {1337, "Exchange", "0x0000000000000000000000000000000000000000", "1"},
        "Agent",
        phantom_agent
    };
}

SignedL1Action HyperliquidSigner::signL1Action(
    const  Action& action,
    std::string active_pool,
    bool is_mainnet,
    std::optional<std::string> vaultAddress,
    std::optional<uint64_t> expiresAfter
) {

    uint64_t nonce = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    //uint64_t nonce = 1782273354130;


    std::vector<uint8_t> hashBytes = getActionHash(action, nonce, active_pool);

    std::string hashHex = bytesToHex(hashBytes);
    std::cout << "hash: " << hashHex << "\n";
    AgentMessage agent = getInfo(is_mainnet, hashBytes);
    L1Payload payload = payLoadL1(agent);
    json full;

    full["primaryType"] = payload.primaryType;

    full["domain"] = {
        {"name", payload.domain.name},
        {"version", payload.domain.version},
        {"chainId", payload.domain.chainId},
        {"verifyingContract", payload.domain.verifyingContract}
    };

    full["message"] = {
        {"source", payload.message.source},
        {"connectionId", payload.message.connectionId}
    };

    std::cout << "data: " << full << " \n";

    // =========================================
    // 5. Generate EIP712 digest
    // =========================================

    std::vector<uint8_t> digest =
        encode_typed_data(full);

    LocalAccount wallet;

    Config cfg = loadConfig();
    wallet.address = cfg.wallet_address;
    wallet.private_key = cfg.private_key;


    std::array<unsigned char, 32> digest_array;

    std::copy(
        digest.begin(),
        digest.end(),
        digest_array.begin()
    );

    // =========================================
    // 7. Convert private key
    // =========================================

    std::vector<uint8_t> pk_bytes =
        hexToBytes(wallet.private_key);

    std::cout << "Private Key: "
        << bytesToHex(pk_bytes)
        << std::endl;

    std::cout << "PK bytes size: "
        << pk_bytes.size()
        << std::endl;

    std::cout << "PK string: "
        << wallet.private_key
        << std::endl;

    std::cout << "PK string length: "
        << wallet.private_key.size()
        << std::endl;

    std::array<unsigned char, 32> privkey;

    std::copy(
        pk_bytes.begin(),
        pk_bytes.end(),
        privkey.begin()
    );

    // =========================================
    // 8. Sign digest
    // =========================================

    Signature sig = sign_hash(digest_array, privkey);

    SignedL1Action result;
    
    result.action = action;
    result.nonce = nonce;
    result.wallet_address =  wallet.address;

    /*result.payload = payload;*/

    result.signature = sig;
    
    return result;
}

std::vector<uint8_t> HyperliquidSigner::pad32(const std::vector<uint8_t>& input)
{
    if (input.size() == 32) return input;

    std::vector<uint8_t> padded(32, 0);
    size_t start_index = 32 - input.size();
    std::copy(input.begin(), input.end(), padded.begin() + start_index);

    return padded;
}

std::vector<uint8_t> HyperliquidSigner::uintToBytes(uint64_t value) 
{
    std::vector<uint8_t> bytes;

    for (int i = 7; i >= 0; --i)
    {
        uint8_t b = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
        bytes.push_back(b);
    }
    return bytes;
}

std::vector<uint8_t> HyperliquidSigner::hash_type(const std::string& signature)
{
    std::vector<uint8_t> type_bytes(signature.begin(), signature.end());
    return keccak256(type_bytes);
}



// Keccak of a string (used for TypeHashes and string fields)
std::vector<uint8_t> HyperliquidSigner::hash_string(const std::string& s) {
    std::vector<uint8_t> data(s.begin(), s.end());
    return keccak256(data);
}

std::vector<uint8_t> HyperliquidSigner::hash_domain(const json& domain)
{
    std::vector<uint8_t> ribbon;

    //1. add domain typehash (The Blueprint Stamp)
    std::vector<uint8_t> type_hash = hash_type("EIP712Domain(string name,string version,uint256 chainId,address verifyingContract)");
    ribbon.insert(ribbon.end(), type_hash.begin(), type_hash.end());

    // 2. Add 'name' (hashed string)
    std::vector<uint8_t> name_hash = hash_string(domain["name"].get<std::string>());
    ribbon.insert(ribbon.end(), name_hash.begin(), name_hash.end());

    // 3. Add 'version' (hashed string)
    std::vector<uint8_t> version_hash = hash_string(domain["version"].get<std::string>());
    ribbon.insert(ribbon.end(), version_hash.begin(), version_hash.end());

    // 4. Add 'chainId' (padded uint256)
    std::vector<uint8_t> chain_id_bytes = pad32(uintToBytes(domain["chainId"].get<uint64_t>()));
    ribbon.insert(ribbon.end(), chain_id_bytes.begin(), chain_id_bytes.end());

    // 5. Add 'verifyingContract' (padded address)
    std::vector<uint8_t> contract_bytes = pad32(hexToBytes(domain["verifyingContract"].get<std::string>()));
    ribbon.insert(ribbon.end(), contract_bytes.begin(), contract_bytes.end());

    // 6. Return the final Keccak hash of the whole 160-byte ribbon
    return keccak256(ribbon);
}


std::vector<uint8_t>
HyperliquidSigner::hash_struct(
    const std::string& primaryType,
    const json& data
)
{
    std::vector<uint8_t> ribbon;

    if (primaryType == "Agent")
    {
        // =========================================
        // 1. typeHash
        // =========================================

        auto type_hash =
            hash_type(
                "Agent(string source,bytes32 connectionId)"
            );


        std::cout
            << "TYPE HASH: "
            << bytesToHex(type_hash)
            << "\n";

        ribbon.insert(
            ribbon.end(),
            type_hash.begin(),
            type_hash.end()
        );


        // =========================================
        // 2. source : string
        // EIP712 => keccak256(bytes(source))
        // =========================================

        std::string source_str =
            data.at("source").get<std::string>();

        std::vector<uint8_t> source_bytes(
            source_str.begin(),
            source_str.end()
        );

        auto source_hash =
            keccak256(source_bytes);

        std::cout
            << "SOURCE HASH: "
            << bytesToHex(source_hash)
            << "\n";

        ribbon.insert(
            ribbon.end(),
            source_hash.begin(),
            source_hash.end()
        );


        // =========================================
        // 3. connectionId : bytes32
        // raw 32 bytes
        // =========================================

        auto conn_id =
            data.at("connectionId")
            .get<std::vector<uint8_t>>();

        std::cout
            << "CONNECTION ID: "
            << bytesToHex(conn_id)
            << "\n";

        if (conn_id.size() != 32)
        {
            throw std::runtime_error(
                "connectionId must be 32 bytes"
            );
        }

        ribbon.insert(
            ribbon.end(),
            conn_id.begin(),
            conn_id.end()
        );
    }

    auto body_hash = keccak256(ribbon);

    std::cout
        << "BODY HASH: "
        << bytesToHex(body_hash)
        << "\n";

    return body_hash;
}

std::vector<uint8_t> HyperliquidSigner::hash_eip712_message(const std::string& primaryType, const json& data) {
    // This is the C++ version of the Python hash_eip712_message.
    // It simply takes the data and the type name, and runs hash_struct.
    return hash_struct(primaryType, data);
}

std::vector<uint8_t> HyperliquidSigner::encode_typed_data(const json& full_message) {
    // 1. Get the Domain Hash (The "header" in Python)
    std::vector<uint8_t> domain_hash = hash_domain(full_message.at("domain"));
    std::cout << "hash domain: " << bytesToHex(domain_hash) << "\n";
    // 2. Get the Message Hash (The "body" in Python)
    // We pass "primaryType" (e.g., "Agent") and the message data
    std::string primaryType = full_message.at("primaryType").get<std::string>();
    std::vector<uint8_t> message_hash = hash_eip712_message(primaryType, full_message.at("message"));
    std::cout << "hash message: " << bytesToHex(message_hash) << "\n";
    // 3. Final EIP-712 Gluing (0x19 + 0x01 + DomainHash + MessageHash)
    std::vector<uint8_t> final_ribbon;

    final_ribbon.push_back(0x19);
    final_ribbon.push_back(0x01);
    final_ribbon.insert(final_ribbon.end(), domain_hash.begin(), domain_hash.end());
    final_ribbon.insert(final_ribbon.end(), message_hash.begin(), message_hash.end());
    
    std::cout << "Final Ribbon: "
        << bytesToHex(final_ribbon)
        << "\n";

    auto digest = keccak256(final_ribbon);

    std::cout << "Digest: "
        << bytesToHex(digest)
        << "\n";

    return digest;
}

Signature HyperliquidSigner:: sign_hash(
    const std::array<unsigned char, 32>& hash,
    const std::array<unsigned char, 32>& privkey
)
{
    Signature sig_out;


    // ==========================================
    // Create secp256k1 context
    // ==========================================

    secp256k1_context* ctx =
        secp256k1_context_create(
            SECP256K1_CONTEXT_SIGN
        );


    // ==========================================
    // Verify private key
    // ==========================================

    if (!secp256k1_ec_seckey_verify(
        ctx,
        privkey.data()
    ))
    {
        throw std::runtime_error(
            "Invalid private key"
        );
    }

    secp256k1_ecdsa_recoverable_signature signature;

    int result =
        secp256k1_ecdsa_sign_recoverable(
            ctx,
            &signature,
            hash.data(),
            privkey.data(),
            nullptr,
            nullptr
        );

    if (!result)
    {
        throw std::runtime_error(
            "Signing failed"
        );
    }


    // ==========================================
    // Serialize compact signature
    // ==========================================

    unsigned char compact_sig[64];

    int recid = 0;

    secp256k1_ecdsa_recoverable_signature_serialize_compact(
        ctx,
        compact_sig,
        &recid,
        &signature
    );


    // ==========================================
    // Extract r
    // ==========================================

    std::memcpy(
        sig_out.r.data(),
        compact_sig,
        32
    );


    // ==========================================
    // Extract s
    // ==========================================

    std::memcpy(
        sig_out.s.data(),
        compact_sig + 32,
        32
    );


    // ==========================================
    // Ethereum v
    // ==========================================

    sig_out.v = recid + 27;


    // ==========================================
    // Cleanup
    // ==========================================

    std::cout << "r=" << bytesToHex(sig_out.r) << '\n';
    std::cout << "s=" << bytesToHex(sig_out.s) << '\n';
    std::cout << "v=" << (int)sig_out.v << '\n';

    secp256k1_context_destroy(ctx);
    return sig_out;
}



