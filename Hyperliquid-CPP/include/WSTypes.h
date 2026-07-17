#ifndef TRADE_H
#define TRADE_H

#include <iostream>
#include <optional>
#include <vector>

struct Trade
{
	std::string coin;
	bool side;
	std::string px;
	std::string sz;
	std::string hash;
	uint64_t time;
	uint64_t tid;
	std::string buyer;
	std::string seller;
};

struct WsLevel {
	std::string px;
	std::string sz;
	uint64_t n;
};

struct WsBbo {
	std::string coin;
	uint64_t time;
	std::optional<WsLevel> bbo1;
	std::optional<WsLevel> bbo2;
};

struct WsBook {
	std::string coin;
	uint64_t time;
	std::vector<WsLevel> asks;
	std::vector<WsLevel> bids;
}; 

#endif