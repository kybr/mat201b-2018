#include "allocore/io/al_App.hpp"
// #include <iostream> // secretly included
using namespace al;
using namespace std;

int main() {
  printf("Vec3f: %luB\n", sizeof(Vec3f));
  printf("Pose: %luB\n", sizeof(Pose));
  printf("float: %luB\n", sizeof(float));
}
