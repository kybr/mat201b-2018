// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use Gamma's tbl (read Table) function
// - Draw a Mesh with the LINE_STRIP primitive
//

#include "Gamma/Oscillator.h"
#include "Gamma/SamplePlayer.h"
#include "Gamma/tbl.h"
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;
using namespace gam;

struct AlloApp : App {
  Mesh windowFunctionMesh;

  AlloApp() {
    int N = 1000;
    float windowFunctionData[N];
    tbl::bartlett(windowFunctionData, N);

    windowFunctionMesh.primitive(Graphics::LINE_STRIP);
    for (int i = 0; i < N; i++) {
      windowFunctionMesh.vertex((float)i / N, windowFunctionData[i]);
      windowFunctionMesh.color(0.1f, 0.5f, 1.0f);
    }

    nav().pos(0.627375, 0.510945, 4.50374);

    initWindow();
    initAudio();
  }

  virtual void onAnimate(double dt) {}

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    g.draw(windowFunctionMesh);
  }

  virtual void onSound(AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      float s = 0;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() { AlloApp().start(); }
