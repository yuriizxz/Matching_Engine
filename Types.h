#include <cstdint>
#include <iostream>

enum class Side { 
		BUY, 
		SELL 
};

enum class OrderType { 
		MARKET, 
		LIMIT, 
		PEG_BID, 
		PEG_OFFER 
};


struct Trade {
  int64_t price;
  int qty;

  void print() {
    
		std::cout << "Trade, price: " << (price / 100.0) << ", qty: " << qty << "\n";
	}
};
