# Matching Engine (Limit Order Book)

Motor de correspondência de ordens (Matching Engine) desenvolvido em C++17. O sistema implementa um Limit Order Book com regras de prioridade Preço-Tempo (FIFO), projetado para simular o roteamento e a execução de ordens em bolsas de valores.

## Funcionalidades

* **Limit Orders:** Ordens de compra e venda com preço limite especificado.
* **Market Orders:** Execução a mercado consumindo a liquidez disponível de forma sequencial até a exaustão.
* **Pegged Orders:** Ordens dinâmicas que rastreiam e acompanham automaticamente o melhor preço de compra (Best Bid) ou de venda (Best Ask) do livro.
* **Modificação de Ordens:** Operações de alteração seguem regras de mercado estritas. Reduzir a quantidade da ordem mantém a sua prioridade na fila. Aumentar a quantidade ou alterar o preço move a ordem para o final da respectiva fila de preço.
* **Cruzamento de Ordens:** Caso uma ordem limit que seja criada esteja apta a realizar um trade com as ordens que já existem no Livro, a trade será realizada a fim de evitar um estado impossível do Livro ( ordens de compra e venda aptas a realizar um trade estando paradas  no livro ). O mesmo vale para ordens modificadas.
* **Cancelamento O(1):** Remoção direta de ordens ativas sem necessidade de varredura nas filas.
* **Tratamento de Entradas:** Validação de input via linha de comando para prevenir estados inválidos (ex: processamento de quantidades e preços nulos ou negativos, falhas de tipagem).

## Arquitetura e Estruturas de Dados

O sistema foi estruturado com base em princípios de separação de responsabilidades, isolando a interface, as regras de negócio e as estruturas de dados. 


### Componentes do Sistema

* **`main.cpp`**: Ponto de entrada da aplicação. Instancia a *Engine* central e transfere o controle do fluxo para o *CLI*.
* **`Types.h`**: Centraliza os enumeradores (`Side`, `OrderType`) e estruturas de dados leves (`Trade`) compartilhados por todo o sistema.
* **`Order.h`**: Define a entidade base `Order`, que armazena o estado imutável e mutável de uma oferta (ID, preço, quantidade, lado, tipo).
* **`OrderBook.h` / `.cpp`**: Camada de dados do livro de ofertas. Abstrai a lógica de manipulação das filas de preço, garantindo a adição, remoção e consulta eficiente, mantendo o encapsulamento das estruturas internas (`std::map`, `std::list`).
* **`PeggedTracker.h` / `.cpp`**: Módulo isolado de rastreamento. Observa as mudanças no topo do livro (*Top of Book*) e contém a lógica para atualizar as Pegged Orders, evitando poluir o motor principal com esta complexidade específica.
* **`Engine.h` / `.cpp`**: O cérebro do sistema (*Core Logic*). Recebe solicitações, executa validações, realiza o algoritmo de cruzamento (Matching) entre compradores e vendedores e orquestra a comunicação entre o `OrderBook` e o `PeggedTracker`.
* **`CLI.h` / `.cpp`**: Camada de Interface (View/Input). Lida exclusivamente com operações de leitura da entrada padrão (`std::cin`), conversão de tipos de dados (`double` para `int64_t`) e tratamento de comandos mal formatados, blindando o restante da aplicação.
* **`CMakeLists.txt`**: Receita de configuração para a geração automatizada da build.


### 1. Representação de Preços (`int64_t`)
O sistema evita o uso de tipos de ponto flutuante (`double` ou `float`) na lógica central para prevenir problemas de precisão e arredondamento binário. Os preços inseridos no terminal são convertidos para `int64_t` (multiplicados por 100 com ajuste de truncamento), garantindo que toda a matemática de cruzamento seja realizada em valores inteiros (centavos).

### 2. Estrutura do OrderBook
Para garantir a regra FIFO e acesso em tempo constante, o livro de ofertas utiliza uma composição de estruturas de dados:
* `std::map<int64_t, std::list<shared_ptr<Order>>>`: Mantém a ordenação dos níveis de preço. Cada preço mapeia para uma lista duplamente encadeada (`std::list`), que preserva a ordem de chegada.
* `std::unordered_map<string, list::iterator>`: Atua como indexador de acesso $O(1)$. Armazena ponteiros diretos (iteradores) para os nós da lista, viabilizando modificações e cancelamentos imediatos sem iteração.

### 3. Data Partitioning no PeggedTracker
O gerenciamento das Pegged Orders particiona o estado em dois mapas distintos (`std::unordered_map` separados para Bids e Offers). Durante uma alteração no topo do livro, o motor itera apenas sobre o lado afetado, reduzindo ciclos de processamento e otimizando a utilização do Cache L1.

## Compilação e Execução

O sistema utiliza o `CMake` como gerador de build.

### Procedimento de Build
```bash
mkdir build
cd build
cmake ..
make
```

### Execução (Modo Interativo)
```bash
./matching_engine
```

### Execução (Lote/Testes)
É possível processar um conjunto de comandos enviando um arquivo de texto diretamente para a entrada padrão:
```bash
./matching_engine < testes.txt
```

## Guia de Comandos (CLI)

O interpretador de comandos aceita a seguinte sintaxe:

| Ação | Sintaxe | Exemplo |
| :--- | :--- | :--- |
| **Limit Order** | `limit <buy/sell> <preço> <quantidade>` | `limit buy 10.50 100` |
| **Market Order** | `market <buy/sell> <quantidade>` | `market sell 50` |
| **Pegged Order** | `peg <bid/offer> <buy/sell> <quantidade>` | `peg bid buy 200` |
| **Modify Order** | `modify <id_da_ordem> <novo_preço> <nova_qtd>` | `modify identificador_1 11.00 150` |
| **Cancel Order** | `cancel order <id_da_ordem>` | `cancel order identificador_2` |
| **Print Book** | `print book` | `print book` |
| **Encerrar** | `quit` ou `exit` | `exit` |
