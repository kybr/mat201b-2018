// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
//  - use lerp for linear
//  - use lerp for exponential out animation
//  - use lerp for exponential in/out animation
//

#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

struct AlloApp : App {
  Mesh m;

  Vec3f pos[3];
  Vec3f destPos[3];

  // required for linear animation
  double time;
  Vec3f startPos0;

  // required for eponential in/out animation
  double amt;

  AlloApp() {
    nav().pos(0, 0, 10);
    addSphere(m);
    m.scale(0.1);
    m.generateNormals();
    initWindow();

    for (int i = 0; i < 3; i++) {
      pos[i] = Vec3f(-1, 1 - i, 0);
      destPos[i] = Vec3f(-1, 1 - i, 0);
    }
    startPos0 = pos[0];
    time = 0.0;

    amt = 0.0;
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    for (int i = 0; i < 3; i++) {
      g.pushMatrix();
      g.translate(pos[i]);
      g.color(i == 0, i == 1, i == 2);
      g.draw(m);
      g.popMatrix();
    }
  }

  virtual void onAnimate(double dt) {
    time += dt;

    // linear animation, must keep track of time and starting position
    if (time <= 1.0)
      pos[0].lerp(destPos[0], time);
    else
      time = 1;

    // exponential out animation, go 10% of the way there every frame
    // starts fast and slows down to a stop
    pos[1].lerp(destPos[1], 0.1);

    // exponential start and finish, slow speed up and slow down
    // also interpolate the amount of interpolation!
    amt = lerp(amt, 0.1, 0.1);
    pos[2].lerp(destPos[2], amt);
  }

  // onMouseMove is when the mouse moves
  virtual void onMouseDown(const ViewpointWindow& w, const Mouse& mouse) {
    Rayd ray = getPickRay(w, mouse.x(), mouse.y());

    // intersect ray with plane at origin
    float t = ray.intersectPlane(Vec3f(0, 0, 0), Vec3f(0, 0, 1));

    // get x position of interseciton point
    float x = ray(t).x;

    // set destination x positions
    for (int i = 0; i < 3; i++) destPos[i].x = x;

    // reset linear animation state
    startPos0.set(pos[0]);
    time = 0;

    // reset expontential in/out state
    amt = 0;
  }

  // other mouse callbacks
  //
  virtual void onMouseMove(const ViewpointWindow& w, const Mouse& m) {}
  virtual void onMouseUp(const ViewpointWindow& w, const Mouse& m) {}
  virtual void onMouseDrag(const ViewpointWindow& w, const Mouse& m) {}
};
int main(int argc, char* argv[]) {
  AlloApp app;
  app.start();
  return 0;
}
