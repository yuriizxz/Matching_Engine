#include "CLI.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>



// >>> LIMIT/MARKET BUY/SELL <PRICE> <QTY>
// >>> CANCEL/MODIFY <QTY>/<ID> <ID>
// >>> PEG BID/OFFER BUY/SELL <QTY>
// >>> PRINT BOOK
// >>>

void CLI::Run(Engine &engine) {

  std::string cmd;
	
	std::cout << ">>> ";

  while (std::cin >> cmd) {
		
		if( cmd == "limit"){
			std::string side_str;
			double price_double;
			int qty;
			
			std::cin >> side_str >> price_double >> qty;

			Side side = (side_str == "buy") ? Side::BUY : Side::SELL;
			int64_t price = static_cast<int64_t>(price_double * 100.0 + 0.5);

			engine.ProcessLimitOrder(side, price, qty);

		}
		else if ( cmd == "market"){

			std::string side_str;
			int qty;

			std::cin >> side_str >> qty;

			Side side = (side_str == "buy") ? Side::BUY : Side::SELL;
			engine.ProcessMarketOrder(side, qty);

		}
		else if ( cmd == "peg"){
			std::string bid_or_offer, side_str;
			int qty;

			std::cin >> bid_or_offer >> side_str >> qty;

			Side side = (side_str == "buy") ? Side::BUY : Side::SELL;
			engine.ProcessPeggedOrder(side, qty);
	
		}
		else if (cmd == "cancel") {
		
			std::string word_order, order_id;

			std::cin >> word_order >> order_id;

			engine.CancelOrder(order_id);


		}
		else if (cmd == "modify"){
			
			std::string order_id;
			double price_double;
			int qty;

			std::cin >> order_id >> price_double >> qty;

			int64_t price = static_cast<int64_t>(price_double * 100.0 + 0.5);
			engine.ModifyOrder(order_id, price, qty);
		
		}
		else if (cmd == "print") {

			std::string word_book;
			std::cin >> word_book;

			engine.GetBook().PrintOrderBook();

		}
		else if (cmd == "exit" || cmd == "quit") {
			break;
		}
		else{

			std::cout << "Comand não reconhecido.\n";

		}

		std::cout << ">>> ";
  }
}
