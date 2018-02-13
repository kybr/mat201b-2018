// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use a Mesh to draw many triangles using a single Mesh object.
//

#include "allocore/io/al_App.hpp"
using namespace al;

struct Color {
  float r, g, b, a;
};

struct HSV {
  float h, s, v;
  RGB toRGB() { ... }
};

struct RGB {
  float r, b, g;
  HSV toHSV() { ... }
};

struct AlloApp : App {
  AlloApp() {
    Color c(HSV(0.5, 1.0, 1.0));
    cout << c.r << endl;
    HSV hsv(c);
    cout << hsv.h << endl;

    // r -> x, g --> y, b -> z
    // h -> angle, s -> radius. v -> z
  }
};

int main() { AlloApp().start(); }
