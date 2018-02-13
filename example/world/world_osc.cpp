// MAT201B
// Winter 2017
// Author: Karl Yerkes
//
// Minimal Example of a "Shared World"
// - using OSC
//

#include "NotchedNoise.hpp"
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace al::osc;
using namespace std;

#define PORT_WE_LISTEN_ON (65123)
#define PORT_WE_SEND_TO (60234)
#define IP_WE_SEND_TO "127.0.0.1"

#define BLOCK_SIZE (256)

// a representation of their state
struct State {
  Pose pose;
  Rayd ray;
};

struct MyApp : PacketHandler, App {
  // their state and our state
  State them;
  Rayd ray;

  // visual stuff
  Mesh line, cone, sphere;
  Light light;
  Material material;

  // audio stuff
  NotchedNoise notchedNoise;

  // spatial audio stuff
  SpeakerLayout speakerLayout = HeadsetSpeakerLayout();
  StereoPanner* panner;
  Listener* listener;
  SoundSource soundSource;
  AudioScene scene;

  MyApp() : scene(BLOCK_SIZE) {
    // make this sound unique
    notchedNoise =
        NotchedNoise(al::rnd::uniform(400.0, 1400.0),
                     al::rnd::uniform(2.0, 5.0), al::rnd::uniform(0.02, 0.3));

    // set up our meshes...
    float hue = al::rnd::uniform();

    line.primitive(Graphics::LINES);
    line.color(HSV(hue, 0.5, 0.8));
    line.vertex(0, 0, 0);
    line.vertex(0, 0, 0);

    cone.color(HSV(hue + 0.3, 0.9, 1));
    addCone(cone);
    cone.scale(0.2);
    cone.generateNormals();

    sphere.color(HSV(hue + 0.6, 0.7, 1));
    addSphere(sphere);
    sphere.scale(0.03);
    sphere.generateNormals();

    // listen for UDP messages on the given port
    oscRecv().open(PORT_WE_LISTEN_ON, "", 0.016, Socket::UDP);
    // make *this app* the message handler
    oscRecv().handler(*this);
    // start listening...
    oscRecv().start();

    // configure the port and ip of our UDP message sender
    oscSend().open(PORT_WE_SEND_TO, IP_WE_SEND_TO, 0.016, Socket::UDP);

    // setup audio spatialization scheme
    panner = new StereoPanner(speakerLayout);
    listener = scene.createListener(panner);
    // listener->compile();  // XXX need this?

    // XXX doppler kills when moving fast!
    soundSource.dopplerType(DOPPLER_NONE);

    // mute sources that at too close
    soundSource.nearClip(lens().near());

    // mute sources that at too far away
    soundSource.farClip(lens().far());

    // how should sounds get quieter with distance?
    // soundSource.law(ATTEN_LINEAR);
    // soundSource.law(ATTEN_INVERSE_SQUARE);
    soundSource.law(ATTEN_INVERSE);

    // add the sound source to the scene
    scene.addSource(soundSource);

    // use block processing rather than pre-sample processing
    scene.usePerSampleProcessing(false);

    initAudio(44100, BLOCK_SIZE);

    // tell Gamma the audio sample playback rate
    gam::Sync::master().spu(audioIO().fps());

    initWindow(Window::Dim(300, 300));
    light.pos(10, 10, 10);
  }

  void onAnimate(double dt) {
    // send our last known ray
    Vec3d& o(ray.o);  // o for origin
    Vec3d& d(ray.d);  // d for direction
    oscSend().send("/ray", o.x, o.y, o.z, d.x, d.y, d.z);

    // send our current position and orientation (pose)
    Vec3d& p = nav().pos();
    Quatd& q = nav().quat();
    oscSend().send("/pose", p.x, p.y, p.z, q[0], q[1], q[2], q[3]);
  }

  void onDraw(Graphics& g) {
    // make a nice gray background like we're using Processing
    g.clearColor(0.13, 0.13, 0.13, 1);
    g.clear(Graphics::COLOR_BUFFER_BIT);

    light();
    material();

    // visualize their ray
    line.vertices()[0] = them.ray.o;
    line.vertices()[1] = them.ray.d;
    g.draw(line);

    // visualize the tip of their ray
    g.pushMatrix();
    g.translate(them.ray.d);
    g.draw(sphere);
    g.popMatrix();

    // visualize their pose
    g.translate(them.pose.pos());
    g.rotate(them.pose.quat());
    g.draw(cone);
  }

  void onMouseMove(const ViewpointWindow& w, const Mouse& m) {
    // figure out where we're pointing
    ray = getPickRay(w, m.x(), m.y());
  }

  virtual void onMessage(Message& m) {
    if (m.addressPattern() == "/ray") {
      // make a note of where our partner is pointing
      m >> them.ray.o.x >> them.ray.o.y >> them.ray.o.z;
      m >> them.ray.d.x >> them.ray.d.y >> them.ray.d.z;
    } else if (m.addressPattern() == "/pose") {
      // make a note of our partner's position and orientation
      double x, y, z, qx, qy, qz, qw;
      m >> x >> y >> z >> qx >> qy >> qz >> qw;
      them.pose.set(Pose(Vec3d(x, y, z), Quatd(qx, qy, qz, qw)));
    } else {
      m.print();
    }
  }

  void onSound(AudioIOData& io) {
    // position the sound source (them)
    Vec3d& p(them.pose.pos());
    soundSource.pos(p.x, p.y, p.z);

    // position the listener (us)
    listener->pose(nav());

    int numFrames = io.framesPerBuffer();
    for (int i = 0; i < numFrames; i++) {
      // Write sample to the source
      soundSource.writeSample(notchedNoise());
    }

    // render this audio scene
    scene.render(io);
  }
};

int main() { MyApp().start(); }
