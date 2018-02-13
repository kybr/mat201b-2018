// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
// - Use Gamma's STFT class
// - Calculate the Short-Time Fourier Transform on an audio stream
//

#include "Gamma/DFT.h"
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;
using namespace gam;

struct AlloApp : App {
  Mesh meter;
  STFT stft;

  AlloApp() {
    nav().pos(-3.11826, 2.22796, 9.83269);

    meter.primitive(Graphics::LINE_STRIP);
    for (int i = 0; i < stft.numBins(); i++)
      meter.vertex(log((float)i / stft.numBins()), 0.0, 0.0);

    initWindow();
    initAudio();
  }

  virtual void onAnimate(double dt) {}

  virtual void onDraw(Graphics& g, const Viewpoint& v) { g.draw(meter); }

  virtual void onSound(AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      float s = io.in(0);

      if (stft(s)) {
        gam::Complex<float>* bin = stft.bins();
        for (unsigned i = 0; i < stft.numBins(); ++i)
          meter.vertices()[i].y = bin[i].mag();
      }

      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() { AlloApp().start(); }
