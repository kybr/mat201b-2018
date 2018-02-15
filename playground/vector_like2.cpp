//#include <string.h>
#include <iostream>
#include <type_traits>
#include <vector>
#include "allocore/math/al_Random.hpp"
#include "allocore/math/al_Vec.hpp"
using namespace std;
using namespace al;

template <typename T>
class vector {
  int size;
  T* data;

 public:
  int size();
};

#define MAX (2000)
struct State {
  unsigned n;
  Vec3f stuff[MAX];

  void fill_stuff(const vector<Vec3f>& v) {
    n = v.size();
    for (unsigned i = 0; i < v.size(); ++i) stuff[i] = v[i];
    // memcpy(&stuff[0], &v[0], sizeof(Vec3f) * v.size());
  }
};

int main() {
  vector<Vec3f> v;
  for (unsigned i = 0; i < 1000; ++i)
    v.push_back(Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()));
  State state;
  state.fill_stuff(v);

  for (unsigned i = 0; i < state.n; ++i) {
    printf("%f == %f\n", v[i].x, state.stuff[i].x);
  }
}
