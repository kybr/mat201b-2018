// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use a Mesh to draw something simple
//

#include "allocore/io/al_App.hpp"
using namespace al;

struct AlloApp : App {
  AlloApp() {
    nav().pos(0, 0, 10);
    initWindow();
  }

  void onDraw(Graphics& g) {
    // make a new Mesh, m. Mesh is the basic unit of
    // drawing in AlloSystem. a Mesh contains vertices
    // colors, normals, texture coordinates, indices and
    // any other "vertex attributes" you care to add.
    //
    Mesh m;

    // this Mesh will render as points.
    //
    m.primitive(Graphics::POINTS);

    // add 7 vertices with random positions. note that
    // because these values are chosen anew on each frame,
    // they will apear as snow.
    //
    for (int i = 0; i < 7; i++)
      m.vertex(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());

    // tell the GPU to draw the Mesh m. nothing will be
    // drawn until this call is made.
    //
    g.draw(m);
  }
};

int main() { AlloApp().start(); }
