#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <map>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

struct Level {
	long long price = 0;
	long long size = 0;
};
class OrderBook
{
public:

	const long long SCALE{ 100000000 };
	std::array<Level, 100> bids;
	std::array<Level, 100> asks;
	int num_bids{ 0 };
	int num_asks{ 0 };


	void update(const nlohmann::json& data);
	void display();

private:
	long long toFixed(const std::string& s);
	void updateSide(std::array<Level, 100>& side, int& count, const nlohmann::json& updates, bool isBid);

};
#endif

