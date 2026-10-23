#include "Engine.h"
#include <algorithm>
#include <cstdint>
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
    // pega o valor mínimo de quantidade entre as partes para realizar o trade
    int trade_qty = std::min(order->qty, best_opposite_order->qty);
    // define o preço favorecendo a parte passiva ( quem ja esta no livro )
    int64_t trade_price = best_opposite_order->price;

    // realiza o trade
    Trade trade{trade_price, trade_qty};
    trade.print();

    // atualiza os valores
    order->qty = order->qty - trade_qty;
    best_opposite_order->qty = best_opposite_order->qty - trade_qty;

    if (best_opposite_order->qty == 0) {
      book.RemoveOrder(best_opposite_order->orderId);
    }
  }
}
void Engine::CancelOrder(const std::string &id) {
  // Verifica se a ordem existe no livro
  auto order = book.GetOrder(id);
  if (!order) {
    // Se a ordem não existe / já foi cancelada / Id é inválido apenas ignora
    return;
  }

  // Remove a ordem em O(1)
  book.RemoveOrder(id);

  std::cout << "Order cancelled\n";
}

void Engine::ModifyOrder(const std::string &id, int64_t new_price,
                         int new_qty) {

  auto order = book.GetOrder(id);
  if (!order) {
    return;
  }

  // Regra 1: A ordem perde prioridade na fila caso tenha seu preço alterado
  // ou se a quantidade de ações foi aumentada
  if (order->price != new_price || order->qty < new_qty) {
    // remove a ordem do livro
    book.RemoveOrder(id);

    // atualiza seus parametros
    order->price = new_price;
    order->qty = new_qty;

    // tenta cruzar a ordem
    MatchOrder(order);

    // Se não for totalmente liquidada é adicionada de volta
    if (order->qty > 0) {
      book.AddOrder(order);
    }

  }
  // Regra 2: Atualiza a ordem e mantém a prioridade caso apenas a quantidade
  // de ações foi diminuida
  else if (order->qty > new_qty) {
    order->qty = new_qty;
  }
}

void Engine::UpdatePeggedOrders() {
  int64_t current_top_bid = 0;
  int64_t current_top_ask = 0;
  if (book.HasBids()) {
    current_top_bid = book.GetBestBidPrice();
  }
  if (book.HasAsks()) {
    current_top_ask = book.GetBestAskPrice();
  }

  if (current_top_bid != last_top_bid && current_top_bid != 0) {
    last_top_bid = current_top_bid;

    for (auto const &[id, order] : peggedTracker.GetPeggedBids()) {
      if (order->price != current_top_bid) {

        if (order->price != 0) {
          book.RemoveOrder(order->orderId);
        }

        order->price = current_top_bid;
        book.AddOrder(order);
      }
    }
  }

  if (current_top_ask != last_top_ask && current_top_ask != 0) {
    last_top_ask = current_top_ask;

    for (auto const &[id, order] : peggedTracker.GetPeggedAsks()) {
      if (order->price != current_top_ask) {

        if (order->price != 0) {
          book.RemoveOrder(order->orderId);
        }

        order->price = current_top_ask;
        book.AddOrder(order);
      }
    }
  }
}
