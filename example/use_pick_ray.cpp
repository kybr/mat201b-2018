// MAT201B
// Fall 2015
// Author: Karl Yerkes
//
// Based on AlloSystem's "pickRay.cpp" example by Tim Wood, 9/19/2014
//

#include "allocore/io/al_App.hpp"
#include "allocore/math/al_Ray.hpp"

using namespace al;

struct MyApp : App {
  // this will be the mesh we use do represent the ball
  //
  Mesh ball_mesh;

  // this will be the position we draw the ball in the scene.
  //
  Vec3f ball_position;

  MyApp() {
    // populate the mesh with a sphere
    //
    addSphere(ball_mesh);

    // disable mouse-based navigation
    //
    navControl().useMouse(false);

    // open a window
    //
    initWindow();
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    // draw the ball at the position 10 units into the scene at the mouses x/y
    //
    g.translate(ball_position);
    g.draw(ball_mesh);
  }

  virtual void onMouseMove(const ViewpointWindow& w, const Mouse& mouse) {
    // make a "ray" based on the mouse x and y
    //
    Rayd mouse_ray = getPickRay(w, mouse.x(), mouse.y());

    // get the position that is 10 units into the scene (from the viewer)
    //
    ball_position = mouse_ray(10.0);
  }
};

int main() { MyApp().start(); }
