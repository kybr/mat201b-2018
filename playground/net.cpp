#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

struct MyApp : App, osc::PacketHandler {
  void onMessage(osc::Message& m) {
    m.print();

    // the message we'll receive consists of a single float
    float v;
    m >> v;

    printf("v = %f\n", v);
  }

  MyApp() {
    initWindow();
    initAudio();
    oscRecv().open(60777, "", 0.016, Socket::UDP);
    oscRecv().handler(*this);
    oscRecv().start();
  }

  void onAnimate(double dt) {}

  void onDraw(Graphics& g) {}

  void onSound(AudioIO& io) {
    while (io()) {
      io.out(0) = 0;
      io.out(1) = 0;
    }
  }

  void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    switch (k.key()) {
      default:
      case '1':
        // reverse time
        timeStep *= -1;
        break;
      case '2':
        // speed up time
        if (timeStep < 1) timeStep *= 2;
        break;
      case '3':
        // slow down time
        if (timeStep > 0.0005) timeStep /= 2;
        break;
      case '4':
        // pause the simulation
        simulate = !simulate;
        break;
    }
  }
};

int main() { MyApp().start(); }
