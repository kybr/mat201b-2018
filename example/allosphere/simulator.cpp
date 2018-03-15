
#include "common.h"

#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/io/al_App.hpp"
#include "alloutil/al_AlloSphereAudioSpatializer.hpp"
#include "alloutil/al_Simulator.hpp"

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

struct MyApp : App, AlloSphereAudioSpatializer, InterfaceServerClient {
  cuttlebone::Maker<State> maker;
  State* state = new State;

  Mesh backgroundMesh;
  Texture backgroundTexture;

  Material material;
  Light light;
  Mesh sphere;

  NotchedNoise notchedNoise = NotchedNoise(1000, 4, 0.1);

  MyApp()
      : maker(Simulator::defaultBroadcastIP()),
        InterfaceServerClient(Simulator::defaultInterfaceServerIP()) {
    // background texture
    lens().near(0.1);
    lens().far(1000);  // increase far clip to 1000 GL Units
    {
      addSphereWithTexcoords(backgroundMesh);
      backgroundMesh.generateNormals();
      Image image;
      if (!image.load(fullPathOrDie("worthersee_klagenfurt.jpg"))) {
        cout << "Failed to read image" << endl;
        exit(-1);
      }
      backgroundTexture.allocate(image.array());
    }

    // agent
    addSphere(sphere);
    sphere.generateNormals();

    light.pos(5, 5, 5);
    nav().pos(0, 0, 20);

    initWindow();

    // init audio....
    AlloSphereAudioSpatializer::initAudio();
    AlloSphereAudioSpatializer::initSpatialization();
    gam::Sync::master().spu(AlloSphereAudioSpatializer::audioIO().fps());
    scene()->addSource(aSoundSource);
    aSoundSource.dopplerType(DOPPLER_NONE);
    aSoundSource.useAttenuation(false);
    // scene()->usePerSampleProcessing(true);
    scene()->usePerSampleProcessing(false);
  }

  void onAnimate(double dt) {
    while (InterfaceServerClient::oscRecv().recv())
      ;  // XXX

    static double t = 0;
    t += dt;
    state->agentPosition.x = -cos(t) * 3;
    state->agentPosition.y = sin(t) * 7;
    state->agentPosition.z = 0;
    state->navPose = nav();

    maker.set(*state);
  }

  void onDraw(Graphics& g) {
    // background texture
    g.lighting(false);
    g.depthMask(false);
    g.pushMatrix();
    g.translate(nav().pos());
    g.rotate(180, 0, 0, 1);
    backgroundTexture.bind();
    g.color(1, 1, 1);
    g.draw(backgroundMesh);
    backgroundTexture.unbind();
    g.popMatrix();

    //

    material();
    light();
    g.translate(state->agentPosition);
    g.draw(sphere);
  }

  SoundSource aSoundSource;
  virtual void onSound(al::AudioIOData& io) {
    aSoundSource.pose().pos(state->agentPosition);
    while (io()) {
      aSoundSource.writeSample(notchedNoise() * 0.05);
    }
    listener()->pose(nav());
    scene()->render(io);
  }

  void onKeyDown(const ViewpointWindow&, const Keyboard& k) {}
};

int main() {
  MyApp app;
  app.AlloSphereAudioSpatializer::audioIO().start();
  app.InterfaceServerClient::connect();
  app.maker.start();
  app.start();
}
