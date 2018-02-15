#include <iostream>
#include <type_traits>
#include "allocore/math/al_Vec.hpp"

#define MAX (1000)
struct VectorLike {
  //  Simple() : size(0) {}  // we want this
  float data[MAX];
  unsigned size;

  // you may have members
  void push_back(float f) {
    data[size] = f;
    size++;
    if (size >= MAX) printf("WARNING: exceeded State size\n");
  }
  void init() { size = 0; }
};

struct State {
  VectorLike thing;
};

int main() {
  if (std::is_pod<al::Vec3f>::value)
    printf("Vec3f is POD!\n");
  else
    printf(":(\n");
  if (std::is_pod<State>::value) printf("It's POD!\n");
  State state;
  state.thing.init();

  state.thing.push_back(0);
  state.thing.push_back(1);
  state.thing.push_back(2);
  state.thing.push_back(3);
  printf("state.thing.size = %u\n", state.thing.size);
}
