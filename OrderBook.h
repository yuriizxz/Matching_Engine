#include "Order.h"
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

// Para facilitar a leitura e a escrita:
using OrderList = std::list<std::shared_ptr<Order>>;
using OrderListIterator = OrderList::iterator;

class OrderBook {
private:
  // Bids ( Do maior para o menor preço)
  std::map<int64_t, OrderList, std::greater<int64_t>> bids;

  // Asks ( Do menor para o maior preço)
  std::map<int64_t, OrderList> asks;

  // Indexador para localizar rapidamente uma ordem pelo seu ID
  std::unordered_map<std::string, OrderListIterator> order_indexer;

public:
  OrderBook() = default;

  // Adiciona uma ordem ao livro de ordens
  void AddOrder(std::shared_ptr<Order> order);

  // Remove uma ordem do livro de ordens
  void RemoveOrder(const std::string &order_id);

  // Imprime o estado atual do livro
  void PrintOrderBook() const;
};
