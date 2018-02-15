// MAT 201B
// Winter 2017
// Author: Karl Yerkes
//
// Minimal OSC broadcaster
//
#include "allocore/al_AlloCore.hpp"

using namespace al;
using namespace al::osc;
using namespace std;

struct MyApp : App {
  int frame = 0;
  bool sendFrame = false;

  MyApp() {
    // send to the local host
    oscSend().open(60777, "255.255.255.255", 0.016, Socket::UDP);
    // send to the local area network broadcast address
    // oscSend().open(60777, "255.255.255.255", 0.016, Socket::UDP);
    // alternative to the above for a specific network..
    // oscSend().open(60777, "192.168.1.255", 0.016, Socket::UDP);
    initWindow();
  }

  void onAnimate(double dt) {
    if (sendFrame) {
      oscSend().send("/frame", frame);
    }
    frame++;
  }

  virtual void onKeyDown(const Keyboard& k) {
    if (k.key() == ' ') {
      sendFrame = !sendFrame;
    }
    oscSend().send("/key", k.key());
  }
};

int main() { MyApp().start(); }
