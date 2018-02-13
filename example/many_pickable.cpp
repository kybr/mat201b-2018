/*
*/

#include "allocore/io/al_App.hpp"
#include "allocore/ui/al_BoundingBox.hpp"
#include "allocore/ui/al_Pickable.hpp"
using namespace al;

#define N (10)
struct Agent : Pickable {};

struct MyApp : public App {
  Light light;
  Material material;

  Mesh mesh;
  Agent agent[N];

  MyApp() {
    nav().pos(0, 0, 10);
    navControl().useMouse(false);

    // Create a red spheres mesh
    addSphere(mesh, 1);
    mesh.generateNormals();
    mesh.color(RGB(1, 0, 0));

    // Initialize BoundingBox
    agent.set(mesh);
    agent.bb.glUnitLength = 10;  // ??? this behaves weird

    // create window
    initWindow();
  }

  void onAnimate(double dt) {}

  void onDraw(Graphics& g) {
    g.lighting(true);
    light();
    material();
    agent.drawMesh(g);

    g.lighting(false);
    agent.drawBB(g);
  }

  virtual void onMouseMove(const ViewpointWindow& w, const Mouse& m) {
    // make a ray from mouse location
    Rayd r = getPickRay(w, m.x(), m.y());
    agent.point(r);
  }
  virtual void onMouseDown(const ViewpointWindow& w, const Mouse& m) {
    Rayd r = getPickRay(w, m.x(), m.y());
    agent.pick(r);
  }
  virtual void onMouseDrag(const ViewpointWindow& w, const Mouse& m) {
    Rayd r = getPickRay(w, m.x(), m.y());
    agent.drag(r);
  }
  virtual void onMouseUp(const ViewpointWindow& w, const Mouse& m) {
    Rayd r = getPickRay(w, m.x(), m.y());
    agent.unpick(r);
  }
};

int main() { MyApp().start(); }
