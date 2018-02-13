// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
// - reset a mesh
// - animate mesh vertices
// - add your own vertices
//

#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

struct AlloApp : App {
  Light light;
  Mesh m;
  double time;

  AlloApp() {
    nav().pos(0, 0, 10);
    light.pos(0, 0, 10);
    addSphere(m);
    m.primitive(Graphics::TRIANGLES);
    m.generateNormals();
    initWindow();

    time = 0.0;
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    light();
    g.draw(m);
  }

  virtual void onAnimate(double dt) {
    time += dt;

    // every 2 seconds
    if (time > 2.0) {
      // reseting the mesh clears current vertices
      m.reset();

      // randomly switch between a sphere mesh and a randomly generated mesh
      if (rnd::prob(0.3))
        addSphere(m);  // add sphere, expects primitive TRIANGLES
      else if (rnd::prob(0.5))
        addSurface(m, 10, 10, 2,
                   2);  // add plane, expects primitive TRIANGLE_STRIP
      else
        for (int i = 0; i < 99; i++)  // add 99 random vertices
          m.vertex(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());

      // randomly switch mesh primitive
      if (rnd::prob(0.3))
        m.primitive(Graphics::LINES);
      else if (rnd::prob(0.5))
        m.primitive(Graphics::TRIANGLE_STRIP);
      else
        m.primitive(Graphics::POINTS);

      time = 0;
    }

    // add random offset to vertices to make them wiggle and deform
    for (int i = 0; i < m.vertices().size(); i++) {
      m.vertices()[i] += rnd::uniformS() * 0.01;
    }

    // must generate normals every frame since we are moving vertices
    m.generateNormals();
  }

  // onMouseMove is when the mouse moves
  virtual void onMouseMove(const ViewpointWindow& w, const Mouse& m) {
    // normalize mouse position from -1.0 to 1.0
    float x = float(m.x()) / w.width() * 2.f - 1.f;
    float y = (float(m.y()) / w.height() * 2.f - 1.f) * -1.f;

    // move light with mouse
    light.pos(Vec3f(x, y, 1.f) * 10.f);
  }

  // other mouse callbacks
  //
  virtual void onMouseDown(const ViewpointWindow& w, const Mouse& m) {}
  virtual void onMouseUp(const ViewpointWindow& w, const Mouse& m) {}
  virtual void onMouseDrag(const ViewpointWindow& w, const Mouse& m) {}
};
int main(int argc, char* argv[]) {
  AlloApp app;
  app.start();
  return 0;
}
