#pragma once
#include "OrderBook.h"
#include "PeggedTracker.h"
#include <memory>
#include <string>

class Engine {
private:
  OrderBook book;
  PeggedTracker peggedTracker;
  uint64_t next_order_id = 0;

  // historico para saber quando o topo das filas mudou
  int64_t last_top_bid = 0;
  int64_t last_top_ask = 0;

  // Função que gera os ids das ordens de maneira incremental
  std::string GenerateOrderId();

  // Função que executa o loop de cruzamento de ordens
  void MatchOrder(std::shared_ptr<Order> order);

  // Função que atualiza as ordens pegged
  void UpdatePeggedOrders();

public:
  Engine() = default;

  // Métodos de criação de ordens chamados pelo usuário
  void ProcessLimitOrder(Side side, int64_t price, int qty);
  void ProcessMarketOrder(Side side, int qty);
  void ProcessPeggedOrder(Side side, int qty);

  // Expõe o livro em vigor
  const OrderBook &GetBook() const;

  // Cancela uma ordem
  void CancelOrder(const std::string &id);

  // Modifica uma ordem
  void ModifyOrder(const std::string &id, int64_t new_price, int new_qty);
};
