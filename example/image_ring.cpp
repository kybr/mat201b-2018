// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Rotate a scene slowly around the origin (in the x/z plane)
// - Use class inheritance to make a version of al::Texture that maintain aspect
// ratio
// - Load a bunch of images and show them in a ring
//
#include <iostream>
#include <iostream>
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

// Make a class that is a Texture plus the aspect ratio of the original image.
//
struct TextureWithAspectRatio : Texture {
  float aspectRatio;
  void onDraw(Graphics& g) { Texture::quad(g, 1.0f, aspectRatio); }
};

struct MyApp : App {
  TextureWithAspectRatio* texture;
  FileList file;
  int memory_used = 0;
  double t = 0;

  MyApp() {
    // find all image files in 'handouts/media' using the new FileList class and
    // glob method.
    //
    SearchPaths searchPaths;
    searchPaths.addSearchPath("../media");
    file = searchPaths.glob("(.*)\\.(png|jpg|jpeg)");
    cout << "Found " << file.count() << " files:" << endl;
    // file.print();

    // Now we know how many images we have, so we can make an array of textures.
    // Then we load each image into a texture.
    //
    texture = new TextureWithAspectRatio[file.count()];
    for (int i = 0; i < file.count(); i++) {
      Image image;
      if (!image.load(file[i].filepath())) {
        cout << "ERROR" << endl;
        exit(1);
      }
      cout << "loaded " << file[i].filepath() << endl;

      // count up and display the memory used so far
      //
      memory_used += image.width() * image.height() * image.bytesPerPixel();
      if (memory_used > 1073741824.0f)
        cout << memory_used / 1073741824.0f << " gigabytes used." << endl;
      else if (memory_used > 1048576.0f)
        cout << memory_used / 1048576.0f << " megabytes used." << endl;
      else if (memory_used > 1024.0f)
        cout << memory_used / 1024.0f << " kilobytes used." << endl;
      else
        cout << memory_used << " bytes used." << endl;

      texture[i].allocate(image.array());
      texture[i].aspectRatio = (float)image.height() / image.width();
    }

    // Make a gray background and open a window.
    //
    background(0.1);
    initWindow();
  }

  virtual void onAnimate(double dt) { t += dt; }

  virtual void onDraw(Graphics& g) {
    // Make the whole scene slowly rotate about the origin
    //
    g.rotate(t, 0, 1, 0);

    // Draw each texture in a ring around the origin. Transformations (i.e.,
    // translate, rotate, scale) on OpenGL happen in the reverse of the order
    // in which they are submitted and they accumulate. Here we translate
    // *before* we rotate so that the pictures are placed in a ring around the
    // origin.
    //
    for (int i = 0; i < file.count(); i++) {
      g.pushMatrix();
      g.rotate(360.0f * i / file.count(), 0, 1, 0);
      g.translate(0, 0, -5);
      texture[i].onDraw(g);
      g.popMatrix();
    }
  }
};

int main() { MyApp().start(); }
