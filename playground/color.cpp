#include "allocore/io/al_App.hpp"
using namespace al;

// the point here was to animate the background color of the window and to
// learn how to use the various color types in AlloSystem: Color, RGB, RGBA,
// HSV, etc.

struct MyApp : App {
  Color bg;
  float t = 0;

  MyApp() {
    initWindow();
    // bg = RGB(1, 0, 0);
  }

  void onAnimate(double dt) {
    t += dt / 10;
    if (t > 1.0) t -= 1;
    //
    //
    bg = HSV(t, abs(sin(t)), 1 - t * t);
    std::cout << t << std::endl;
    // set the background
    background(bg);
  }
  void onDraw(Graphics& g, const Viewpoint& vp) {}
};

int main() { MyApp().start(); }
