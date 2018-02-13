// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use Gamma's SineD<> and Accum<> classes
//

#include "Gamma/Oscillator.h"
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;
using namespace gam;

struct AlloApp : App {
  SineD<> sined;
  Accum<> timer;

  AlloApp() {
    timer.freq(2.0f);
    sined.set(rnd::uniform(220.0f, 880.0f), 0.5f, 1.0f);
    initWindow();
    initAudio();
  }

  virtual void onAnimate(double dt) {}

  virtual void onDraw(Graphics& g, const Viewpoint& v) {}

  virtual void onSound(AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      if (timer()) {
        sined.set(rnd::uniform(220.0f, 880.0f), 0.5f, 1.0f);
      }
      float s = sined();
      io.out(0) = s;
      io.out(1) = s;
    }
  }

  virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    if (k.key() == ' ') {
      cout << "Spacebar pressed" << endl;
    }
  }
};

int main() { AlloApp().start(); }
