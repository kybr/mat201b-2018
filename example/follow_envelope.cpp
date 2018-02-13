// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to visualize the amplitude envelope of a beating heart
//
#include "Gamma/SamplePlayer.h"
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;
using namespace gam;

struct MyApp : App {
  Mesh m;
  float visualLoudnessMeasure;
  SamplePlayer<float, gam::ipl::Linear, phsInc::Loop> samplePlayer;

  MyApp() {
    addSphere(m);
    SearchPaths searchPaths;
    searchPaths.addSearchPath("..");
    string filePath = searchPaths.find("heart_beat.wav").filepath();
    cout << filePath << endl;
    samplePlayer.load(filePath.c_str());
    initAudio();
    gam::Sync::master().spu(audioIO().fps());
    initWindow();
    nav().pos(0, 0, 9);
    background(HSV(0.7, 0.5, 0.5));
  }

  virtual void onDraw(Graphics& g) {
    float x = rnd::uniformS();  // [-1, 1)
    float y = rnd::uniformS();

    g.translate(Vec3f(x, y, 0) * visualLoudnessMeasure);
    g.color(HSV(visualLoudnessMeasure, 1, visualLoudnessMeasure));
    g.draw(m);
  }

  virtual void onSound(AudioIOData& io) {
    visualLoudnessMeasure = 0;
    while (io()) {
      float s = samplePlayer();
      if (abs(s) > visualLoudnessMeasure) visualLoudnessMeasure = abs(s);
      io.out(0) = s;
      io.out(1) = s;
    }
    visualLoudnessMeasure *= 1.2;
  }
};

int main() { MyApp().start(); }
