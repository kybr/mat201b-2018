// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - This code loads and displays multiple images, simultaneously
//

#include <cassert>  // gets you assert()
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

const char* fileList[] = {"chart.png", "surround.jpg", "pluto.png"};

// we define N automatically given the list above. N is the number of file names
// in the list.
//
#define N (sizeof(fileList) / sizeof(fileList[0]))

// i put this stuff in a function because i'll do it over and over in a loop
// below.
//
string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

struct AlloApp : App {
  // we'll have a texture for each image.
  //
  Texture texture[N];

  AlloApp() {
    for (int i = 0; i < N; i++) {
      Image image;
      if (!image.load(fullPathOrDie(fileList[i], ".."))) {
        cerr << "failed to load " << fileList[i] << endl;
        exit(1);
      }

      // XXX if you want access to the pixels inside the ith image, this is
      // where to put your code. see the Homework 2, Problem 1 starter code
      // (handouts/hw2/p1.cpp) for details on accessing pixel data.

      // this is how to copy image data into an al::Texture object.
      //
      texture[i].allocate(image.array());
    }

    // set the position and orientation of the viewer so they can see all the
    // images clearly. i found these numbers by printing the position of the
    // nav while i used the keyboard to get a good position.
    //
    nav().pos(Vec3d(4.2, 0.5, 1.9));
    nav().quat(Quatd(0.96, 0.00, 0.29, 0.00));

    initWindow();
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    for (int i = 0; i < N; i++) {
      // translate the image back (down the -z axis) a little. note that this
      // translate accumulates. use g.pushMatrix()/g.popMatrix() to prevent
      // this.
      //
      g.translate(0, 0, -2);

      // this is a convenience method of the texture class. an al::Texture
      // knows how to draw itself on a quadrilateral of a given al::Graphics
      // object. this call does not honor the original aspect ratio of the
      // images.
      //
      texture[i].quad(g);
    }
  }
};

int main() { AlloApp().start(); }
