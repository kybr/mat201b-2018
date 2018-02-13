// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// This is based on an example I made for the 2012 class.
//
// Shows how to:
// - Procedurally build mesh
// - Use TRIANGLE_STRIP primitive
// - Pass by reference
// - Ask OpenGL something about the current graphics hardware
//

#include "allocore/io/al_App.hpp"
using namespace al;
#include <cassert>
using namespace std;

void addTexturedCylinder(Mesh& m, float radius = 1.0f, int segmentCount = 16) {
  m.primitive(Graphics::TRIANGLE_STRIP);
  for (int segment = 0; segment < segmentCount + 1; segment++) {
    float t = (float)segment / segmentCount;
    float x = radius * cos(t * M_2PI);
    float z = radius * sin(t * M_2PI);

    //
    //
    m.vertex(x, 0, z);
    m.vertex(x, 1, z);

    // inward-facing
    //
    m.normal(-x, 0, -z);
    m.normal(-x, 0, -z);

    // white
    //
    m.color(1, 1, 1);
    m.color(1, 1, 1);

    // texture coordinates
    //
    m.texCoord(t, 0);
    m.texCoord(t, 1);
  }
}

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

struct MyApp : App {
  Mesh cylinder;
  Texture texture;

  MyApp() {
    // add a textured cylinder to the mesh
    //
    addTexturedCylinder(cylinder, 5, 32);

    // scale the mesh to make it taller (y dimension)
    //
    cylinder.scale(1, 2, 1);

    // load an image into the texture
    //
    Image image;
    image.load(fullPathOrDie("worthersee_klagenfurt.jpg", ".."));
    texture.allocate(image.array());

    // place the viewer "up" a little and open a window
    //
    nav().pos(0, 1, 0);
    initWindow();
  }

  // keep track of time in a variable: t
  //
  double t = 0;
  virtual void onAnimate(double dt) { t += dt; }

  virtual void onDraw(Graphics& g) {
    // draw the cylinder with the texture bound
    //
    g.rotate(t, 0, 1, 0);
    texture.bind();
    g.draw(cylinder);
    texture.unbind();

    // if this is the first time in the draw loop, ask opengl a question
    //
    static bool firstTime = true;
    if (firstTime) {
      firstTime = false;
      int size;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
      cout << "Your maximum texture size is " << size << " pixels" << endl;
    }
  }
};

int main() { MyApp().start(); }
