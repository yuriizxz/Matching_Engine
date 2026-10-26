#include "CLI.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>


// >>> LIMIT/MARKET BUY/SELL <PRICE> <QTY>
// >>> CANCEL/MODIFY <QTY>/<ID> <ID>
// >>> PEG BID/OFFER BUY/SELL <QTY>
// >>> PRINT BOOK
// >>>

void CLI::Run(Engine &engine) {

  std::string cmd;

  std::cout << ">>> ";
//Começa o loop lendo a primeira entrada antes do primeiro espaço
  while (std::cin >> cmd) {

    if (cmd == "limit") {
      std::string side_str;
      double price_double;
      int qty;
			//Lê o resto da string
      std::cin >> side_str >> price_double >> qty;

			//Caso seja digitado uma entrada invalida ( erro de digitaçao, letra quando se esperava numero e etc), trata o erro imprimindo uma mensagem e retornando o fluxo de entrada para o estado normal
      if (std::cin.fail()) {
        std::cout << "Erro: Formato invalido. Esperado: limit <buy/sell> <preco> <quantidade>\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      } else {
        Side side = (side_str == "buy") ? Side::BUY : Side::SELL;
        int64_t price = static_cast<int64_t>(price_double * 100.0 + 0.5);

        engine.ProcessLimitOrder(side, price, qty);
      }
    } else if (cmd == "market") {

      std::string side_str;
      int qty;

      std::cin >> side_str >> qty;
      if (std::cin.fail()) {
        std::cout << "Erro: Formato invalido. Esperado: market <buy/sell> <quantidade>\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      } else {
      Side side = (side_str == "buy") ? Side::BUY : Side::SELL;
      engine.ProcessMarketOrder(side, qty);
			}
    } else if (cmd == "peg") {
      std::string bid_or_offer, side_str;
      int qty;

      std::cin >> bid_or_offer >> side_str >> qty;
			if (std::cin.fail()) {
        std::cout << "Erro: Formato invalido. Esperado: peg <bid/offer> <buy/sell> <quantidade>\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      } else {
      Side side = (side_str == "buy") ? Side::BUY : Side::SELL;
      engine.ProcessPeggedOrder(side, qty);
			}

    } else if (cmd == "cancel") {

      std::string word_order, order_id;

      std::cin >> word_order >> order_id;
			  if (std::cin.fail()) {
        std::cout << "Erro: Formato invalido. Esperado: cancel order indentificador_<id>\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      } else {
      engine.CancelOrder(order_id);
			}
    } else if (cmd == "modify") {

      std::string order_id;
      double price_double;
      int qty;

      std::cin >> order_id >> price_double >> qty;
			   if (std::cin.fail()) {
        std::cout << "Erro: Formato invalido. Esperado: modify identificador_<id> <preco> <quantidade>\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      } else {
			//Faz o cast para transformar o double para int64_t truncando o numero para cima adicionando 0.5, economizando a chamada da funçao std::round()
      int64_t price = static_cast<int64_t>(price_double * 100.0 + 0.5);
      engine.ModifyOrder(order_id, price, qty);
			}
    } else if (cmd == "print") {

      std::string word_book;
      std::cin >> word_book;
      if (std::cin.fail()) {
        std::cout << "Erro: Formato invalido. Esperado: print book\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      } else {
      engine.GetBook().PrintOrderBook();
			}
    } else if (cmd == "exit" || cmd == "quit") {
			// Ou apertando ctrl + c
      break;
    } else {
			//Caso o primeiro input digitado seja inválido 
      std::cout << "Comando não reconhecido.\n";
    }

    std::cout << ">>> ";
  }
}
