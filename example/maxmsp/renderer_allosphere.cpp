// MAT201B
// Fall 2015
// Author: Karl Yerkes
//

#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"

using namespace al;
using namespace al::osc;
using namespace std;

struct AlloApp : OmniStereoGraphicsRenderer, PacketHandler {
  Recv recv;
  Mesh sphere;
  Vec3f position;

  AlloApp() {
    recv.open(60777, "", 0.016, Socket::UDP);
    recv.handler(*this);
    recv.start();
    addSphere(sphere);
    position.zero();
  }

  virtual void onMessage(Message& m) {
    if (m.addressPattern() == "/reset") {
      // do nothing
    } else if (m.addressPattern() == "/lonlat") {
      float lat, lon;
      m >> lon;
      m >> lat;
      lon *= M_PI / 180.0f;
      lat *= M_PI / 180.0f;
      position = Vec3f(-cos(lat) * cos(lon), sin(lat), cos(lat) * sin(lon));
    } else if (m.addressPattern() == "/background") {
      float r, g, b, a;
      m >> r;
      m >> g;
      m >> b;
      m >> a;
      omni().clearColor() = Color(r, g, b, a);
    } else {
      m.print();
    }
  }

  virtual void onAnimate(double dt) {}

  virtual void onDraw(Graphics& g) {
    g.translate(position);
    g.scale(0.1);
    g.draw(sphere);
  }
};

int main() { AlloApp().start(); }
