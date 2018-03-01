#include <iostream>
#include <thread>

void dothings() { printf("DONE!\n"); }

int main() {
  std::thread t(dothings);
  printf("GOT HERE!\n");
  t.join();
}
