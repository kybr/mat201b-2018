#include <unistd.h>
#include <cstdlib>
#include <future>
#include <iostream>
#include <vector>

int twice(int m) {
  usleep(rand() / RAND_MAX * 0.2 + 10000);
  return 2 * m;
}

int main() {
  std::vector<std::future<int>> futures;

  for (int i = 0; i < 10; ++i) {
    futures.push_back(std::async(twice, i));
  }

  // retrive and print the value stored in the future
  for (auto &e : futures) {
    std::cout << e.get() << std::endl;
  }

  return 0;
}
