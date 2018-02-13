
#include "Gamma/Envelope.h"
#include "Gamma/Filter.h"
#include "Gamma/Gamma.h"
#include "Gamma/Noise.h"
#include "Gamma/Oscillator.h"
using namespace gam;

struct NotchedNoise {
  Accum<> tmr;      // Timer for resetting envelope
  NoisePink<> src;  // Source to envelope
  Decay<> env;      // Exponentially decaying envelope
  Biquad<> bq;      // Biquad filter
  NotchedNoise(double cutoff = 1000.0, double res = 4.0, double period = 0.1) {
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
