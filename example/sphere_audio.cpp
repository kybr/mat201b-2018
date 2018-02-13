#include <Gamma/SamplePlayer.h>
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/io/al_App.hpp"
#include "alloutil/al_AlloSphereAudioSpatializer.hpp"
#include "alloutil/al_Simulator.hpp"

using namespace al;
using namespace std;
using namespace gam;

struct State {
  int frame;
};

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

struct SphereApp : App, AlloSphereAudioSpatializer, InterfaceServerClient {
  cuttlebone::Maker<State, 9000> maker;

  SamplePlayer<float, gam::ipl::Linear, phsInc::Loop> samplePlayer;

  SoundSource soundSource;

  State* state;
  SphereApp()
      : maker(Simulator::defaultBroadcastIP()),
        InterfaceServerClient(Simulator::defaultInterfaceServerIP()) {
    state = new State;
    memset(state, 0, sizeof(State));

    samplePlayer.load(fullPathOrDie("gibber_chimes.wav").c_str());

    AlloSphereAudioSpatializer::initAudio();
    AlloSphereAudioSpatializer::initSpatialization();
    gam::Sync::master().spu(AlloSphereAudioSpatializer::audioIO().fps());
    scene()->addSource(soundSource);
    scene()->usePerSampleProcessing(false);

    InterfaceServerClient::setNav(nav());
    InterfaceServerClient::setLens(lens());
  }

  virtual void onAnimate(double dt) {}

  virtual void onDraw(Graphics& g) {}

  virtual void onSound(AudioIOData& io) {
    static int sampleCount = 0;
    while (io()) {
      double t = (sampleCount / io.fps());
      float s = samplePlayer();
      soundSource.pose(Pose(Vec3f(3 * sin(t), 0, 3 * cos(t)), Quatf()));
      soundSource.writeSample(s);
      sampleCount++;
    }

    listener()->pose(Pose(Vec3f(), Quatf()));
    scene()->render(io);
  }
};

int main() {
  SphereApp app;
  app.AlloSphereAudioSpatializer::audioIO().start();  // start audio
  app.InterfaceServerClient::connect();  // handshake with interface server
  app.maker.start();                     // it's important to call this.
  app.start();
}
