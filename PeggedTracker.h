#pragma once
#include "Order.h"
#include <memory>
#include <unordered_map>
#include <vector>

class PeggedTracker {
private:
  // tabelas que guardaram as orders segundo seu Id
  std::unordered_map<std::string, std::shared_ptr<Order>> pegBids;
  std::unordered_map<std::string, std::shared_ptr<Order>> pegAsks;

public:
  // Adiciona a pegged order a sua respectiva tabela
  void AddPeggedOrder(std::shared_ptr<Order> order) {
    if (order->side == Side::BUY) {
      pegBids[order->orderId] = order;
    } else {
      pegAsks[order->orderId] = order;
    }
  }

  void RemovePeggedOrder(const std::string &id) {
    // Remove em O(1) a order da respectiva tabela
    pegBids.erase(id);
    pegAsks.erase(id);
  }

  const std::unordered_map<std::string, std::shared_ptr<Order>>
  GetPeggedBids() {
    return pegBids;
  }

  const std::unordered_map<std::string, std::shared_ptr<Order>>
  GetPeggedAsks() {
    return pegAsks;
  }
};
