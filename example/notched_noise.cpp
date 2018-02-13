// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//

#include "Gamma/Envelope.h"
#include "Gamma/Filter.h"
#include "Gamma/Gamma.h"
#include "Gamma/Noise.h"
#include "Gamma/Oscillator.h"
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;
using namespace gam;

struct NotchedNoise {
  Accum<> tmr;      // Timer for resetting envelope
  NoisePink<> src;  // Source to envelope
  Decay<> env;      // Exponentially decaying envelope
  Biquad<> bq;      // Biquad filter
  NotchedNoise(double cutoff, double res, double period) {
    bq.type(BAND_PASS);
    bq.freq(cutoff);
    bq.res(res);  // Set resonance of filter
    tmr.period(period);
    tmr.phaseMax();
  }

  float operator()() {
    if (tmr()) {
      env.decay(0.2);  // Set decay length to 1 second
      env.reset(0.2);  // Reset envelope and specify amplitude
    }

    // Envelope source
    return bq(src() * env());
  }
};

struct MyApp : App {
  NotchedNoise notchedNoise = NotchedNoise(1000, 4, 0.1);

  MyApp() {
    initAudio();
    initWindow();
    gam::Sync::master().spu(audioIO().fps());
  }

  void onSound(AudioIOData& io) {
    while (io()) {
      io.out(0) = io.out(1) = notchedNoise();
    }
  }
};

int main() { MyApp().start(); }
