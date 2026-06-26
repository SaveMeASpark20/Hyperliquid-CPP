#ifndef HYPERLIQUIDTYPES_H
#define HYPERLIQUIDTYPES_H

#include <iostream>
#include <variant>
#include <string>
//struct Order
//{
//    uint32_t asset;
//    bool isBuy;
//    std::string size;
//    std::string price;
//    bool reduceOnly;
//    std::string tif; // "Gtc", "Ioc", "Alo"
//};



struct BuilderInfo
{
	std::string b;
	int f;
};



struct OrderWire
{
	int asset;
	bool is_buy;
	std::string px;
	std::string sz;
	bool reduce_only;
	std::string tif = "Ioc";
	std::string cloid;
};

struct UpdateLeverageAction {
	std::string type;
	int asset;
	bool is_cross;
	int leverage;
};


struct OrderAction
{
	std::vector<OrderWire> orders;
	std::string grouping = "na";

	BuilderInfo* builder = nullptr;
};


using Action = std::variant<
	OrderAction,
	UpdateLeverageAction
>;




#endif