#pragma once
#include <string>
#include "Types.h"

class Order {
	public:
		std::string orderId;
		OrderType orderType;
		Side side;
		int64_t price;
		int qty;

		Order(std::string orderId, OrderType orderType, Side side, int64_t price, int qty)
		: orderId(orderId), orderType(orderType), side(side), price(price), qty(qty) {}
};
