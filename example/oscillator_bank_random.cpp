// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//
//

#include "Gamma/Filter.h"
#include "Gamma/Oscillator.h"
//#include "Gamma/DFT.h"
#include "allocore/io/al_App.hpp"

const float range[] = {0.5, 4.0, 8.0, 13.0, 30.0, 50.0};
const int N = sizeof(range) / sizeof(float) - 1;
const float fourOctaves = 2 * 2 * 2 * 2;

struct AlloApp : al::App {
  gam::Sine<> sine[N];
  gam::OnePole<> filter[N];
  gam::Accum<> timer;

  AlloApp() {
    for (int i = 0; i < N; i++) filter[i].freq(7);

    for (int i = 0; i < N; i++) {
      filter[i] = fourOctaves * al::rnd::uniform(range[i], range[i + 1]);
    }
    timer.period(1.0f);
    initAudio();
  }

  virtual void onSound(al::AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      if (timer()) {
        for (int i = 0; i < N; i++)
          filter[i] = fourOctaves * al::rnd::uniform(range[i], range[i + 1]);
      }

      for (int i = 0; i < N; i++) {
        sine[i].freq(filter[i]());
      }

      float s = 0;
      for (int i = 0; i < N; i++) {
        s += sine[i]();
      }
      s /= N;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() { AlloApp().start(); }
