// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//
//

#include "Gamma/Filter.h"
#include "Gamma/Oscillator.h"
#include "allocore/io/al_App.hpp"
#include "alloutil/al_AlloSphereAudioSpatializer.hpp"

using namespace al;

#define u(...) al::rnd::uniform(__VA_ARGS__)
#define s(...) al::rnd::uniformS(__VA_ARGS__)

const int N = 20;

struct AlloApp : al::App, al::AlloSphereAudioSpatializer {
  al::SoundSource source[N];
  gam::Sine<> sine[N];
  gam::OnePole<> frequency[N];
  gam::Accum<> timer;

  Mesh points;

  AlloApp() {
    for (int i = 0; i < N; i++) {
      frequency[i].freq(12);
      frequency[i] = 55 * pow(2, u(1.0, 8.0));
    }
    timer.period(1.7f);

    AlloSphereAudioSpatializer::initAudio();
    AlloSphereAudioSpatializer::initSpatialization();
    gam::Sync::master().spu(AlloSphereAudioSpatializer::audioIO().fps());

    for (int i = 0; i < N; i++) {
      scene()->addSource(source[i]);
      Vec3f t(s(), s(), s());
      t *= 10;
      points.vertex(t);
      source[i].pos(t.x, t.y, t.z);
      source[i].law(ATTEN_INVERSE);
    }

    scene()->usePerSampleProcessing(false);

    initWindow();
  }

  virtual void onAnimate(double dt) { nav().pos(Vec3f()); }
  virtual void onDraw(Graphics& g) { g.draw(points); }

  virtual void onSound(al::AudioIOData& io) {
    while (io()) {
      if (timer())
        for (int i = 0; i < N; i++)
          frequency[i] = round(55 * pow(2, u(1.0, 8.0)));

      for (int i = 0; i < N; i++) {
        sine[i].freq(frequency[i]());
        source[i].writeSample(sine[i]() / N);
      }
    }

    listener()->pose(nav());
    scene()->render(io);
  }
};

int main() {
  AlloApp app;
  app.AlloSphereAudioSpatializer::audioIO().start();
  app.start();
}
