// Hashes for EIP-712 Domain Components
// keccak256("EIP712Domain(string name,string version,uint256 chainId,address verifyingContract)")
const char* DOMAIN_TYPEHASH = "8b73c3c69bb8fe3d512ecc4cf759cc79239f7b179b0ffacaa9a75d522b39400f";

std::vector<uint8_t> HyperliquidSigner::getDomainHash() {
    std::vector<uint8_t> data;
    data.insert(data.end(), hexToBytes(DOMAIN_TYPEHASH).begin(), hexToBytes(DOMAIN_TYPEHASH).end());
    data.insert(data.end(), keccak256({ 'E','x','c','h','a','n','g','e' }).begin(), keccak256({ 'E','x','c','h','a','n','g','e' }).end());
    data.insert(data.end(), keccak256({ '1' }).begin(), keccak256({ '1' }).end());

    // ChainID 1337 packed to 32 bytes big-endian
    uint8_t chainIdBytes[32] = { 0 };

    chainIdBytes[31] = 0x39; // 1337 = 0x0539
    chainIdBytes[30] = 0x05;
    data.insert(data.end(), std::begin(chainIdBytes), std::end(chainIdBytes));

    // VerifyingContract (Address 0 packed to 32 bytes)
    uint8_t addrBytes[32] = { 0 };
    data.insert(data.end(), std::begin(addrBytes), std::end(addrBytes));

    return keccak256(data);
}


// keccak256("Agent(string source,bytes32 connectionId)")
const char* AGENT_TYPEHASH = "203597d391f681a8ca46c0628e93231e6b8c9d4647a6cf8f5b85a3a7b7520e50";

std::vector<uint8_t> HyperliquidSigner::getAgentHash(const std::vector<uint8_t>& connectionId, bool isMainnet) {
    std::vector<uint8_t> data;
    data.insert(data.end(), hexToBytes(AGENT_TYPEHASH).begin(), hexToBytes(AGENT_TYPEHASH).end());

    // CRITICAL: Source is 'a' (mainnet) or 'b' (testnet)
    std::vector<uint8_t> sourceHash = keccak256({ (uint8_t)(isMainnet ? 'a' : 'b') });
    data.insert(data.end(), sourceHash.begin(), sourceHash.end());
    data.insert(data.end(), connectionId.begin(), connectionId.end());

    return keccak256(data);
}


std::vector<uint8_t> digest;
digest.push_back(0x19);
digest.push_back(0x01);
auto domain = getDomainHash();
auto agent = getAgentHash(connectionId, isMainnet);
digest.insert(digest.end(), domain.begin(), domain.end());
digest.insert(digest.end(), agent.begin(), agent.end());

auto finalSigningHash = keccak256(digest);


const char* EIP712_DOMAIN_TYPEHASH = "8b73c3c69bb8fe3d512ecc4cf759cc79239f7b179b0ffacaa9a75d522b39400f";
const char* AGENT_TYPEHASH = "203597d391f681a8ca46c0628e93231e6b8c9d4647a6cf8f5b85a3a7b7520e50";

std::vector<uint8_t> getDomainHash() {
    std::vector<uint8_t> data;
    appendBytes(data, hexToBytes(EIP712_DOMAIN_TYPEHASH));
    appendBytes(data, keccak256("Exchange")); // Name
    appendBytes(data, keccak256("1"));        // Version
    appendBigEndian(data, (uint256_t)1337);   // ChainId
    appendBytes(data, std::vector<uint8_t>(32, 0)); // VerifyingContract (0x0...)
    return keccak256(data);
}

std::vector<uint8_t> getFinalHash(const std::vector<uint8_t>& actionHash, bool isMainnet) {
    std::vector<uint8_t> agentData;
    appendBytes(agentData, hexToBytes(AGENT_TYPEHASH));
    // CRITICAL: Source is 'a' (Mainnet) or 'b' (Testnet)
    appendBytes(agentData, keccak256(isMainnet ? "a" : "b"));
    appendBytes(agentData, actionHash); // This is your connectionId

    std::vector<uint8_t> finalDigest;
    finalDigest.push_back(0x19);
    finalDigest.push_back(0x01);
    appendBytes(finalDigest, getDomainHash());
    appendBytes(finalDigest, keccak256(agentData));

    return keccak256(finalDigest);
}
