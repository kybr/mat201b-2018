#include "allocore/al_Allocore.hpp"
#include "allocore/io/al_App.hpp"
#include "allocore/sound/al_Ambisonics.hpp"

using namespace al;

struct Agent : public SoundSource, public Nav {
  double oscPhase = 0, oscEnv = 1;
  double move;
  double rate;
  Agent() {
    rate = rnd::uniform(5, 11);
    Nav::pos(-rnd::uniformS(), 0, -4);
    move = rnd::uniform(0.05, 0.5);
  }
  virtual ~Agent() {}

  virtual void onProcess(AudioIOData& io) {
    // for each sample for just this agent
    while (io()) {
      float s = sin(oscPhase * M_2PI);

      // if (oscEnv < 0.00001) {
      //  oscEnv = 1;
      //  oscPhase = 0;
      //}

      oscPhase += rate / io.framesPerSecond();
      if (oscPhase >= 1) oscPhase -= 1;
      writeSample(s * 0.2);
    }
  }

  virtual void onUpdateNav() {
    smooth(0.9);

    spin(M_2PI / 360, M_2PI / 397, rnd::uniformS(0.01));
    moveF(move);

    step(0.9);

    // both SoundSource and Nav have a pose
    SoundSource::pose(*this);
  }

  virtual void onDraw(Graphics& g) {
    g.pushMatrix();
    g.scale(10);
    g.mesh().primitive(g.LINES);
    addWireBox(g.mesh());
    g.draw();
    g.popMatrix();

    g.pushMatrix();
    g.multMatrix(directionMatrix());
    g.begin(g.TRIANGLES);
    float ds = 0.5;

    g.color(1, 1, 1);
    g.vertex(0, 0, ds * 2);
    g.color(1, 1, 0);
    g.vertex(ds / 2, 0, -ds);
    g.color(1, 0, 1);
    g.vertex(-ds / 2, 0, -ds);

    g.color(1, 1, 1);
    g.vertex(0, 0, ds * 2);
    g.color(0, 1, 1);
    g.vertex(0, ds / 2, -ds);
    g.color(1, 0, 1);
    g.vertex(-ds / 2, 0, -ds);

    g.color(1, 1, 1);
    g.vertex(0, 0, ds * 2);
    g.color(1, 1, 0);
    g.vertex(ds / 2, 0, -ds);
    g.color(0, 1, 1);
    g.vertex(0, ds / 2, -ds);
    g.end();
    g.popMatrix();
  }
};

struct MyApp : App {
  // each of these is a sound source
  std::vector<Agent> agents;

  // audio spatialization stuff
  AudioScene scene;
  Listener* listener;
  SpeakerLayout speakerLayout;
  AmbisonicsSpatializer* spat;

  MyApp() : scene(256) {
    agents.resize(3);

    nav().pos(Vec3d(0, 0, -4));
    // nav().smooth(0.95);

    // Set speaker layout
    const int numSpeakers = 2;
    Speaker speakers[numSpeakers] = {
        Speaker(0, 45, 0),
        Speaker(1, -45, 0),
    };

    speakerLayout.addSpeaker(speakers[0]);
    speakerLayout.addSpeaker(speakers[1]);

    // Create spatializer
    spat = new AmbisonicsSpatializer(speakerLayout, 2, 1);

    // per sample processing is less
    // efficient than per buffer (default),
    // but provides higher quality Doppler
    // scene.usePerSampleProcessing(true);
    scene.usePerSampleProcessing(false);

    // Create listener to render audio
    listener = scene.createListener(spat);

    for (unsigned i = 0; i < agents.size(); ++i) scene.addSource(agents[i]);

    initWindow();
    initAudio();
  }

  void onAnimate(double dt) {
    // nav().step(5);
  }

  void onSound(AudioIOData& io) {
    for (unsigned i = 0; i < agents.size(); ++i) {
      io.frame(0);
      agents[i].onUpdateNav();
      agents[i].onProcess(io);
    }

    listener->pose(nav());
    io.frame(0);
    scene.render(io);
  }

  virtual void onDraw(Graphics& g) {
    for (unsigned i = 0; i < agents.size(); ++i) {
      agents[i].onDraw(g);
    }
  }
};

int main(int argc, char* argv[]) { MyApp().start(); }
