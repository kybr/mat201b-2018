// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - This code loads and displays a textured triangle
// - How to update texture coordinates
//

#include <unistd.h>
#include <cassert>  // gets you assert()
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

struct AlloApp : App {
  Texture texture;
  Mesh triangle;

  AlloApp() {
    Image image;
    const char* fileName = "chart.png";
    if (!image.load(fullPathOrDie(fileName))) {
      cerr << "failed to load " << fileName << endl;
      exit(1);
    }
    texture.allocate(image.array());

    triangle.primitive(Graphics::TRIANGLES);

    triangle.vertex(0, 0);
    triangle.vertex(1, 0);
    triangle.vertex(0.5, 0.707);

    triangle.texCoord(0, 0);
    triangle.texCoord(1, 0);
    triangle.texCoord(0.5, 0.707);

    nav().pos(1, 0, 5);
    initWindow();
  }

  double t = 0;
  virtual void onAnimate(double dt) {
    t += dt;
    // animate the horizontal texture coordinate
    //
    triangle.texCoord2s()[2][0] = (sin(t) + 1) / 2;
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    texture.bind();
    g.draw(triangle);
    texture.unbind();
  }
};

int main() {
  char buf[1000];
  printf("%s\n", getcwd(buf, 1000));
  AlloApp().start();
}
