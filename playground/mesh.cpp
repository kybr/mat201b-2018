#include "allocore/io/al_App.hpp"
using namespace al;

struct Frame : Mesh {
  Frame() {
    primitive(Graphics::LINES);
    // x-axis
    vertex(0, 0, 0);
    color(1, 1, 1);
    vertex(1, 0, 0);
    color(1, 0, 0);
    // y-axis
    vertex(0, 0, 0);
    color(1, 1, 1);
    vertex(0, 1, 0);
    color(0, 1, 0);
    // z-axis
    vertex(0, 0, 0);
    color(1, 1, 1);
    vertex(0, 0, 1);
    color(0, 0, 1);
  }
};

struct Circle : Mesh {
  Circle() {
    primitive(Graphics::LINE_LOOP);
    for (int i = 0; i < 100; i++) {
      vertex(sin(M_PI * 2 * i / 100), cos(M_PI * 2 * i / 100));
      color(1, 1, 1);
    }
  }
};

struct MyApp : App {
  float t = 0;
  Frame frame;
  Circle c;
  Mesh mesh;
  MyApp() {
    initWindow();

    // use a mesh like an array of Vec3f
    c.vertices()[0].z = 5;

    const Buffer<Circle::Vertex>& b = c.vertices();
    c.vertices()[60] = cross(c.vertices()[10], c.vertices()[90]);

    // addTetrahedron(mesh);
  }

  void onAnimate(double dt) {
    t += dt / 10;
    if (t > 1.0) t -= 1;
  }
  void onDraw(Graphics& g, const Viewpoint& vp) {
    g.draw(frame);
    g.draw(c);
    g.draw(mesh);
  }
};

int main() { MyApp().start(); }
