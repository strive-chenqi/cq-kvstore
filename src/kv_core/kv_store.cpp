#include "kv_store.h"

int main() {

  NetworkType type = NetworkType::ASIO; // 选择具体的实现

  auto network = NetworkFactory::createNetwork(type);

  network->run();

  return 0;
}