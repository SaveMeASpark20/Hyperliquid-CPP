#ifndef HYPERLIQUIDSIGNER_H
#define HYPERLIQUIDSIGNER_H

#ifndef SECP256K1_STATIC
#define SECP256K1_STATIC
#endif

//#include <secp256k1.h>
//#include <secp256k1_recovery.h> 

#include <optional>
#include <vector>
#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <msgpack.hpp>  
#include <iostream>
#include <variant>

#include "HyperliquidTypes.h"


using json = nlohmann::json;


template<typename Container>
inline std::string bytesToHex(const Container& bytes)
{
		std::stringstream ss;

		for (uint8_t b : bytes)
		{
			ss << std::hex
				<< std::setw(2)
				<< std::setfill('0')
				<< static_cast<int>(b);
		}

		return ss.str();
}



struct AgentMessage {
	std::string source;
	std::vector<uint8_t> connectionId;

	std::string serialize() const {
		return "{\"source\":\"" + source + "\",\"connectionId\":\"" + bytesToHex(connectionId) + "\"}";
	}
};



struct Domain {
	uint32_t chainId;
	std::string name;
	std::string verifyingContract;
	std::string version;

	std::string serialize() const {
		return "{\"chainId\":" + std::to_string(chainId) +
			",\"name\":\"" + name +
			"\",\"verifyingContract\":\"" + verifyingContract +
			"\", \"version\":\"" + version + "\"}";
	}
};

const std::string EIP712_TYPES_JSON =
"{\"Agent\":[{\"name\":\"source\",\"type\":\"string\"},{\"name\":\"connectionId\",\"type\":\"bytes32\"}],"
"\"EIP712Domain\":[{\"name\":\"name\",\"type\":\"string\"},{\"name\":\"version\",\"type\":\"string\"},"
"{\"name\":\"chainId\",\"type\":\"uint256\"},{\"name\":\"verifyingContract\",\"type\":\"address\"}]}";

struct L1Payload {
	Domain domain;
	std::string primaryType;
	AgentMessage message;

	std::string serialize() const {
		return "{"
			"\"domain\":" + domain.serialize() + ","
			"\"types\":" + EIP712_TYPES_JSON + ","
			"\"primaryType\":\"" + primaryType + "\","
			"\"message\":" + message.serialize() +
			"}";
	}
};

//struct LocalAccount {
//	std::string address;
//	std::string private_key;
//
//	struct SignedAction {
//		std::string wallet_address;
//		L1Payload payload;
//	};
//};


//using MsgPackMap =
//msgpack::type::assoc_vector<
//	std::string,
//	msgpack::type::variant
//>;

//struct BuilderInfo
//{
//	std::string b;
//	int f;
//};
//
//
//
//struct OrderWire
//{
//	int asset;
//	bool is_buy;
//	std::string px;
//	std::string sz;
//	bool reduce_only;
//
//	// only supporting limit/Gtc for now
//	std::string tif = "Ioc";
//
//	std::string cloid;
//};
//
//struct OrderAction
//{
//	std::vector<OrderWire> orders;
//	std::string grouping = "na";
//
//	BuilderInfo* builder = nullptr;
//};
//
//struct UpdateLeverageAction {
//	std::string type;
//	int asset;
//	bool is_cross;
//	int leverage;
//};
//
//
//using Action = std::variant<
//	OrderAction,
//	UpdateLeverageAction
//>;

struct SignableMessage {
	uint8_t version = 0x01;
	std::vector<uint8_t> header; // Domain Hash
	std::vector<uint8_t> body;   // Message Hash
};

// The final "signed" output container
struct SignedMessage {
	std::vector<uint8_t> message_hash; // Final 32-byte digest
	std::string r;
	std::string s;
	int v;
};

struct Signature
{
	std::array<unsigned char, 32> r;
	std::array<unsigned char, 32> s;

	int v;
};


struct SignedL1Action {
	Action action;
	uint64_t nonce;
	std::string wallet_address;
	L1Payload payload;
	Signature signature;
	std::optional<std::string> vaultAddress;
	std::optional<uint64_t> expiresAfter;
};

struct Config {
	std::string private_key;
	std::string wallet_address;
};

//update_leverage_action = {
//	"type": "updateLeverage",
//	"asset" : self.info.name_to_asset(name),
//	"isCross" : is_cross,
//	"leverage" : leverage,
//}

class HyperliquidSigner
{
	public:

		SignedL1Action signL1Action(
			const  Action& action,
			std::string active_pool,
			bool is_mainnet,
			std::optional<std::string> vaultAddress = std::nullopt,
			std::optional<uint64_t> expiresAfter = std::nullopt
		);

		/*std::vector<uint8_t> encode_typed_data(const json& full_message);*/

		//std::vector<uint8_t> hash_domain(const json& domain);
		/*std::vector<uint8_t> hash_struct(const std::string& primaryType, const json& message);*/

		json toJson(const SignedL1Action& signedAction);

	private:
		std::vector<uint8_t> packAction(const Action& action);

		std::vector<uint8_t> getActionHash(const Action& action, uint64_t nonce, const std::string& vault_address, std::optional<uint64_t> expires_after);

		AgentMessage getInfo(bool is_mainnet, std::vector<uint8_t> hash);
		L1Payload payLoadL1(AgentMessage phantom_agent);
		std::vector<uint8_t> hexToBytes(std::string hex);
		/*Signature sign_inner(const std::vector<uint8_t>& private_key, const json& data);*/

		std::vector<uint8_t> uintToBytes(uint64_t value);
		std::vector<uint8_t> hash_type(const std::string& signature);

		std::vector<uint8_t> hash_domain(const json& domain);
		//std::string bytesToHex(const std::vector<uint8_t>& bytes);
		std::vector<uint8_t> encode_typed_data(const json& full_message);
		std::vector<uint8_t> hash_struct(const std::string& primaryType, const json& data);
		//nlohmann::json sign_inner(const LocalAccount& wallet, const nlohmann::json& data);
		Signature sign_hash(const std::array<unsigned char, 32>& hash, const std::array<unsigned char, 32>& privkey);
		std::vector<uint8_t> keccak256(const std::vector<uint8_t>& input);

		std::vector<uint8_t>pad32(const std::vector<uint8_t>& input);
		std::vector<uint8_t> hash_string(const std::string& s);
		std::vector<uint8_t> hash_eip712_message(const std::string& primaryType, const json& data);
		json actionToJson(const OrderAction& action);
		json actionToJson(const UpdateLeverageAction& action);
		std::vector<uint8_t> packActionImpl(const OrderAction& action);
		std::vector<uint8_t> packActionImpl(const UpdateLeverageAction& action);
		Config loadConfig();
};

//struct HyperliquidOrder {
//    uint32_t asset;
//    bool isBuy;
//    uint64_t limitPrice;
//    uint64_t size;
//    uint64_t nonce;
//    bool reduceOnly;
//};
//
//
//class HyperliquidSigner 
//{
//    secp256k1_context* ctx;
//
//public:
//    HyperliquidSigner();
//
//    ~HyperliquidSigner();
//
//    static std::vector<uint8_t> hexToBytes(std::string hex);
//
//    std::vector<uint8_t> sign(const std::vector<uint8_t>& messageHash, const std::vector<uint8_t>& privKey);
//
//    static std::vector<uint8_t> keccak256(const std::vector<uint8_t>& data);
//
//    std::vector<uint8_t> hashOrder(const HyperliquidOrder& order);
//
//    std::vector<uint8_t> getDomainHash(bool isTestnet);
//
//    std::vector<uint8_t> getFinalHash(const std::vector<uint8_t>& orderHash);
//
//    std::vector<uint8_t> getActionTypeHash();
//
//    std::vector<uint8_t> getFinalActionHash(const std::vector<uint8_t>& orderhash, uint64_t nonce, bool isTestnet);
//
//    std::string toHex(const std::vector<uint8_t>& bytes);
//
//    uint64_t getCurrentNonce();
//
//    void debugOrderAction(uint64_t sharedNonce);
//
//};



#endif


