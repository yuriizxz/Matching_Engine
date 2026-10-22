#include "OrderBook.h"
#include <string>

class Engine {
private:
  OrderBook book;
  uint64_t next_order_id = 0;

  // Função que gera os ids das ordens de maneira incremental
  std::string GenerateOrderId();

  // Função que executa o loop de cruzamento de ordens
  void MatchOrder(std::shared_ptr<Order> order);

public:
  Engine() = default;

  // Métodos de criação de ordens chamados pelo usuário
  void ProcessLimitOrder(Side side, int64_t price, int qty);
  void ProcessMarketOrder(Side side, int qty);

  // Expõe o livro em vigor
  const OrderBook &GetBook() const;
};
