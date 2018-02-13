// MAT201B - Why onAnimate?
// Author: Karl Yerkes (2015)
//
//   This is an example of "the right way" to separate drawing code and
//   animation (or simulation) code. Note that there are 6 windows, but only
//   one onDraw() callback function. onDraw() is shared. Each window uses
//   onDraw() to render gets called once for each frame, for each window.
//
//   Observe that because we count frames in onAnimate() each of the windows is
//   (mostly) in sync with respect to the other windows. If each window is to
//   act as it's own view into a temporally coherent world, then this example
//   wins. See "the wrong way".
//

#include "allocore/graphics/al_Font.hpp"
#include "allocore/io/al_App.hpp"
#include "allocore/io/al_File.hpp"
using namespace al;
using namespace std;

struct AlloApp : App {
  unsigned frame;
  FilePath filePath;

  AlloApp() {
    frame = 0;

    SearchPaths searchPaths;
    searchPaths.addSearchPath(".");
    filePath = searchPaths.find("VeraMono.ttf");
    if (!filePath.valid()) {
      cerr << "Font file not found!" << endl;
      exit(1);
    }

    // make 6 windows, each with 1 FPS
    //
    initWindow(Window::Dim(0, 300, 200, 200), "window 1", 1);
    initWindow(Window::Dim(200, 300, 200, 200), "window 2", 1);
    initWindow(Window::Dim(400, 300, 200, 200), "window 3", 1);
    initWindow(Window::Dim(600, 300, 200, 200), "window 4", 1);
    initWindow(Window::Dim(800, 300, 200, 200), "window 5", 1);
    initWindow(Window::Dim(1000, 300, 200, 200), "window 6", 1);
  }

  virtual void onAnimate(double dt) {
    // count up frames in here
    //
    frame++;
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    // render the frame number
    //
    Font* font = new Font(filePath.filepath(), 72);
    g.translate(-1, -1, -10);
    g.scale(0.01);
    g.blendAdd();
    g.depthMask(false);
    font->renderf(g, "%u", frame);
    g.blendOff();
    g.depthTesting(true);
  }
};

int main() { AlloApp().start(); }
