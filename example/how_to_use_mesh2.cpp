// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use a Mesh to draw many triangles using a single Mesh object.
//

#include "allocore/io/al_App.hpp"
using namespace al;

// make N triangles
//
#define N (10)

struct AlloApp : App {
  Mesh aFewRandomTriangles;

  AlloApp() {
    // draw with the 'triangle' primitive. that means that every triple of
    // verticies forms a single triangle.
    //
    aFewRandomTriangles.primitive(Graphics::TRIANGLES);
    for (int i = 0; i < N; i++) {
      Vec3f position(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
      position *= 3;        // spread out the trianlges a little bit
      float radius = 0.2f;  // make the triangles smaller

      // add three vertices at a time. each triple forms a triangle. note that
      // the z position is always zero. these triangles are all in the x/y
      // plane.
      //
      for (int i = 0; i < 3; i++)
        aFewRandomTriangles.vertex(
            position + Vec3f(radius * sin(2 * 3.141592f * i / 3.0f),
                             radius * cos(2 * 3.141592f * i / 3.0f), 0.0f));

      // add color to the mesh. 3 * N random colors.
      //
      for (int i = 0; i < 3 * N; i++)
        aFewRandomTriangles.color(HSV(rnd::uniform(), 1.0f, 1.0f));
    }

    nav().pos(0, 0, 10);
    initWindow();
  }

  void onDraw(Graphics& g) { g.draw(aFewRandomTriangles); }
};

int main() { AlloApp().start(); }
