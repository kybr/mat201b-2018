// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//
// example of a X (frames, blocks, simulation steps) "per second" counter
//

#include "allocore/io/al_App.hpp"
#include "alloutil/al_FPS.hpp"
using namespace al;
using namespace std;

struct XPS {
  double period = 0;
  unsigned int count = 0;
  bool operator()(double dt) {
    count++;
    period += dt;
    if (period > 1.0) {
      period -= 1.0;
      return true;
    }
    return false;
  }
  unsigned int get() {
    unsigned int t = count;
    count = 0;
    return t;
  }
};

struct AlloApp : App {
  XPS animate;
  AlloApp() { initWindow(Window::Dim(500, 400), "test fps", 23); }

  virtual void onAnimate(double dt) {
    if (animate(dt)) {
      cout << "animate: " << animate.get() << endl;
    }
  }
};

int main() { AlloApp().start(); }
