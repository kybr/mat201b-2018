#include "allocore/io/al_App.hpp"
// #include <iostream> // secretly included
using namespace al;
using namespace std;

Vec3f toVec(Quatf q) { return Vec3f(q.x, q.y, q.z); }

int main() {
  Vec3f z_axis(0, 0, 1), y_axis(0, 1, 0);

  Quatf qz(z_axis), qy(y_axis);

  cout << "before: " << endl;
  qz.print();
  toVec(qz).print();
  cout << endl;

  qz.slerpTo(qy, 0.5);

  cout << "after: " << endl;
  qz.print();
  toVec(qz).print();
  cout << endl;
}
