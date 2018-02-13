// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - render a sphere inside another sphere with some "translucency"
//

#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

struct AlloApp : App {
  Light light;
  Material material;
  Mesh small;
  Mesh large;

  AlloApp() {
    nav().pos(0, 0, 5);
    light.pos(10, 10, 10);
    addSphere(small, 1.0, 64, 64);
    for (int i = 0; i < small.vertices().size(); i++)
      small.color(Color(HSV(0.7, 0.5, 1.0), 0.6));
    small.generateNormals();
    small.scale(0.1);
    addSphere(large, 1.0, 64, 64);
    for (int i = 0; i < large.vertices().size(); i++)
      large.color(Color(HSV(0.9, 0.5, 1.0), 0.2));
    large.generateNormals();
    initWindow();
    background(0.07);
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    g.depthTesting(true);
    g.blending(true);
    g.blendModeTrans();

    material.specular(light.diffuse() * 0.2);  // Specular highlight, "shine"
    material.shininess(50);  // Concentration of specular component [0,128]

    light();
    material();

    g.pushMatrix();
    g.translate(0, 0.3, -0.5);
    g.draw(small);
    g.popMatrix();

    g.draw(large);
  }

  double t = 0;
  virtual void onAnimate(double dt) {
    t += 0.005;
    if (t > 1.0) t -= 1.0;
    for (int i = 0; i < large.vertices().size(); i++) large.colors()[i].a = t;
  }
};

int main() { AlloApp().start(); }
