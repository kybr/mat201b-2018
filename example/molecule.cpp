// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Build a structure procedurally
//

#include "allocore/io/al_App.hpp"
using namespace al;

struct Molecule {
  Mesh ball, bar;
  Molecule() {
    addSphere(ball);
    addCylinder(bar);
    ball.generateNormals();
    bar.generateNormals();
    bar.scale(0.2, 0.2, 1.5);
  }

  void onDraw(Graphics& g) {
    g.draw(ball);
    g.pushMatrix();
    g.translate(0, -0.5, -1.7);
    g.draw(bar);
    g.popMatrix();
    g.pushMatrix();
    g.translate(0, +0.5, -1.7);
    g.draw(bar);
    g.popMatrix();
    g.pushMatrix();
    g.translate(0, 0, -3.0);
    g.draw(ball);
    g.popMatrix();
  }
};

struct AlloApp : App {
  Light light;
  Material material;
  Molecule molecule;

  AlloApp() {
    nav().pos(0, 0, 10);
    light.pos(5, 5, 5);
    initWindow();
  }

  void onDraw(Graphics& g) {
    light();
    material();
    molecule.onDraw(g);
  }
};

int main() { AlloApp().start(); }
