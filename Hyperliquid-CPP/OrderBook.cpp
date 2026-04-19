#include <OrderBook.h>
#include <iostream>
#include <charconv> 

//long long OrderBook::toFixed(const std::string& s)
//{
//	return static_cast<long long>(std::stod(s) * SCALE);
//}

long long OrderBook::toFixed(const std::string& s)
{
	double val = 0.0;

	// We use from_chars on the raw character buffer.
	// It takes [start_pointer, end_pointer, output_variable]

	auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), val);

	if (ec != std::errc())
	{
		try
		{
			val = std::stod(s);
		}
		catch (...) 
		{
			return 0;
		}
	}

	return static_cast<long long>(val * SCALE);
}

void OrderBook::update(const nlohmann::json& data)
{
	// GUARD: Check if this JSON is actually an l2Book update
		// If it doesn't have "channel" == "l2Book", skip this message!
	if (!data.contains("channel") || data["channel"] != "l2Book") {
		return;
	}
	// Hyperliquid L2 data has 'levels' which is [bids, asks]
	const auto& levels = data["data"]["levels"];


	/*
	Base on what i red the hyperliquid will sends the snapshot not the update of the price, it will gave us top 20sh
	and that way we don't know if bids[0] and asks[0] is updated. so if the price goes down and bids still holds last up price we will having a problem same on asks
	
	*/
	

	num_bids = 0;
	num_asks = 0;

	// Also, wipe the first few slots to be safe
	bids[0] = { 0, 0 };
	asks[0] = { 0, 0 };



	updateSide(bids, num_bids, levels[0], true);
	updateSide(asks, num_asks, levels[1], false);
}

void OrderBook::updateSide(std::array<Level, 100>& side, int& count, const nlohmann::json& updates, bool isBid)
{
	for (const auto& item : updates)
	{

		// DEBUG STEP: Reset count to 0 for every message. 
// This forces the array to act like a fresh snapshot.
		
		long long px = toFixed(item["px"].get<std::string>());
		long long sz = toFixed(item["sz"].get<std::string>());

		//1. SEARCH: check if price exists
		int found_idx = -1;
		for (int i { 0 }; i < count; ++i)
		{
			if (side[i].price == px)
			{
				found_idx = i;
				break;
			}
		}
		if (found_idx != -1)
		{
			if (sz == 0)
			{
				side[found_idx] = side[count - 1];
				side[count - 1].price =  0;
				side[count - 1].size = 0;
				
				count--;
			}
			else side[found_idx].size = sz;
		}
		else if (sz > 0 && count < 100)
		{
			side[count] = { px, sz };
			count++;
		}

	}
	if (isBid)
	{
		std::sort(side.begin(), side.begin() + count,
			[](const Level& a, const Level& b) { return a.price > b.price; });
	}
	else
	{
		std::sort(side.begin(), side.begin() + count,
			[](const Level& a, const Level& b) { return a.price < b.price; });
	}
}

void OrderBook::display() {
	if (num_bids == 0 || num_asks == 0) return;
	
	// Use [0] to get the best prices from the sorted array
	double bestAsk = static_cast<double>(asks[0].price) / SCALE;
	double bestBid = static_cast<double>(bids[0].price) / SCALE;

	std::cout << "\n--- Task 3: Zero-Allocation Array Book ---" << "\n";
	std::cout << "Best Ask: " << bestAsk << " | Size: " << (double)asks[0].size / SCALE << "\n";
	std::cout << "Best Bid: " << bestBid << " | Size: " << (double)bids[0].size / SCALE << "\n";
	std::cout << "Spread (Ticks): " << (asks[0].price - bids[0].price) << "\n";
}