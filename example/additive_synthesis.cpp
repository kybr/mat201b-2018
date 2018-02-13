// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//
//

#include "Gamma/Filter.h"
#include "Gamma/Oscillator.h"
#include "allocore/io/al_App.hpp"

const int N = 1000;

struct AlloApp : al::App {
  gam::Sine<> sine[N];
  gam::OnePole<> frequency[N];
  gam::OnePole<> amplitude[N];
  gam::Accum<> timer;

  AlloApp() {
    for (int i = 0; i < N; i++) {
      frequency[i].freq(7);
      frequency[i] = 55 * pow(2, al::rnd::uniform(1.0, 8.0));
      amplitude[i] = al::rnd::uniform(1.0);
    }
    timer.period(1.0f);
    initAudio();
  }

  virtual void onSound(al::AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      if (timer())
        for (int i = 0; i < N; i++) {
          frequency[i] = round(55 * pow(2, al::rnd::uniform(1.0, 8.0)));
          amplitude[i] = al::rnd::uniform(1.0);
        }
      for (int i = 0; i < N; i++) sine[i].freq(frequency[i]());

      float s = 0;
      for (int i = 0; i < N; i++) s += sine[i]() * amplitude[i]();
      s /= N;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() { AlloApp().start(); }
