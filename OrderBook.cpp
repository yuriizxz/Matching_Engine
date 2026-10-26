#include "OrderBook.h"
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

void OrderBook::AddOrder(std::shared_ptr<Order> order) {
  if (order->side == Side::BUY) {
    // Coloca a ordem no final da lista do preço especificado
    bids[order->price].push_back(order);

    // Armazena o iterador da ordem no indexador
    order_indexer[order->orderId] = std::prev(bids[order->price].end());
  } else {
    // Coloca a ordem de venda no final da lista do preço especificado
    asks[order->price].push_back(order);
    // Armazena o iterador da ordem no indexador
    order_indexer[order->orderId] = std::prev(asks[order->price].end());
  }
}

void OrderBook::RemoveOrder(const std::string &order_id) {

  auto itr = order_indexer.find(order_id);

  if (itr == order_indexer.end()) {
    return; // Ordem não encontrada
  }

  // Obtém o iterador da ordem a partir do indexador
  OrderListIterator order_iter = itr->second;
  std::shared_ptr<Order> order = *order_iter;

  // Remove a ordem da lista de ordens correspondente
  if (order->side == Side::BUY) {
    bids[order->price].erase(order_iter);

    // Se a fila de preço ficar vazia, removemos o nó para economizar memória
    if (bids[order->price].empty()) {
      bids.erase(order->price);
    }
  } else {
    asks[order->price].erase(order_iter);

    if (asks[order->price].empty()) {
      asks.erase(order->price);
    }
  }

  // Remove a ordem do indexador
  order_indexer.erase(itr);
}

// função auxiliar para formatar o preço para duas casas decimais
static std::string FormatPrice(int64_t price) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << (price / 100.0);
  return oss.str();
}

void OrderBook::PrintOrderBook() const {

  std::cout << "Ordens de Compra    | Ordens de Venda\n";
  std::cout << "--------------------|-----------------\n";

  // Extrai as ordens de compra e venda em vetores para facilitar a impressão
  std::vector<std::string> buy_lines;
  for (auto const &[price, order_list] : bids) {
    // Itera pela lista respeitando a ordem de chegada (FIFO) e imprimindo
    // individualmente cada ordem para uma mesma faixa de preço ( sem juntar as
    // quantidades de shares para uma mesma faixa de preço)
    for (auto const &order : order_list) {
      buy_lines.push_back(std::to_string(order->qty) + " @ " +
                          FormatPrice(order->price));
    }
  }

  std::vector<std::string> sell_lines;
  for (auto const &[price, order_list] : asks) {
    for (auto const &order : order_list) {
      sell_lines.push_back(std::to_string(order->qty) + " @ " +
                           FormatPrice(order->price));
    }
  }

  // descobre qual dos sides tem mais linhas para que seja imprimido tudo
  size_t max_lines = std::max(buy_lines.size(), sell_lines.size());

  // imprime linha por linha
  for (size_t i = 0; i < max_lines; i++) {
    std::string left_side = (i < buy_lines.size()) ? buy_lines[i] : "";
    std::string right_side = (i < sell_lines.size()) ? sell_lines[i] : "";
    std::cout << left_side << std::setw(15) << " |  " << right_side << "\n";
  }
  std::cout << "\n";
}

std::shared_ptr<Order> OrderBook::GetBestOppositeOrder(Side side) const {
  if (side == Side::BUY) {
    // retorna ponteiro nullo caso não haja ordens de venda
    if (asks.empty())
      return nullptr;
    // Retorna o ponteiro da primeira ordem do menor preço de venda
    return asks.begin()->second.front();

  } else {
    if (bids.empty())
      return nullptr;
    return bids.begin()->second.front();
  }
}

std::shared_ptr<Order> OrderBook::GetOrder(std::string id) const {
  // procura a ordem na tabela segundo o id
  auto itr = order_indexer.find(id);

  // Verifica se foi encontrada ordem, se sim retorna o ponteiro da ordem
  if (itr != order_indexer.end()) {
    return *(itr->second);
  }

  // Retorna ponteiro nulo caso a ordem não esteja na tabela
  return nullptr;
}
