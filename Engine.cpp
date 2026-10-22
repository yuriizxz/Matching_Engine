#include "Engine.h"
#include <algorithm>
#include <string>

const OrderBook &Engine::GetBook() const { return book; }

std::string Engine::GenerateOrderId() {
  return "identificador_" + std::to_string(next_order_id++);
}

void Engine::ProcessLimitOrder(Side side, int64_t price, int qty) {
  // cria um indentificador para o ordem
  std::string id = GenerateOrderId();
  // Cria a ordem com os parâmetros passados
  std::shared_ptr<Order> new_order =
      std::make_shared<Order>(id, OrderType::LIMIT, side, price, qty);

  // Imprime que a ordem foi criada
  std::cout << "Order created: " << (side == Side::BUY ? "buy" : "sell")
            << " @ " << (price / 100.0) << " " << id << "\n";

  // Faz o cruzamento da ordem com as que estão no book
  MatchOrder(new_order);

  // Se a ordem não for totalmente liquidada, seu restante é adicionado ao book
  if (new_order->qty > 0) {
    book.AddOrder(new_order);
  }
}

void Engine::ProcessMarketOrder(Side side, int qty) {
  // Cria um identificador para log
  std::string id = "market_" + std::to_string(next_order_id++);

  // Para ordem market, o preço é indiferente portanto 0.
  std::shared_ptr<Order> new_order =
      std::make_shared<Order>(id, OrderType::MARKET, side, 0, qty);

  // Faz o cruzamento da ordem
  MatchOrder(new_order);
}

void Engine::MatchOrder(std::shared_ptr<Order> order) {
  while (order->qty > 0) {

    std::shared_ptr<Order> best_opposite_order =
        book.GetBestOppositeOrder(order->side);
    if (best_opposite_order == nullptr) {
      break;
    }

    if (order->orderType == OrderType::LIMIT) {
      // Comprador que pagar menos que os vendedores oferecem
      if (order->side == Side::BUY &&
          order->price < best_opposite_order->price) {
        break;
      }
      // Vendedor quer receber mais que os compradores oferecem
      if (order->side == Side::SELL &&
          order->price > best_opposite_order->price) {
        break;
      }
    }
    //pega o valor mínimo de quantidade entre as partes para realizar o trade
    int trade_qty = std::min(order->qty, best_opposite_order->qty);
		//define o preço favorecendo a parte passiva ( quem ja esta no livro )
    int64_t trade_price = best_opposite_order->price;

		//realiza o trade
    Trade trade{trade_price, trade_qty};
    trade.print();
		
		//atualiza os valores
    order->qty = order->qty - trade_qty;
    best_opposite_order->qty = best_opposite_order->qty - trade_qty;

    if (best_opposite_order->qty == 0) {
      book.RemoveOrder(best_opposite_order->orderId);
    }
  }
}
