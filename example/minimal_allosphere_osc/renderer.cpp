// MAT201B
// Original authors: Karl Yerkes and Matt Wright (2012)
// Author: Karl Yerkes (2015)
//

#include "allocore/io/al_App.hpp"

using namespace al;
using namespace al::osc;
using namespace std;

#include "common.hpp"  // XXX

struct AlloApp : App, PacketHandler {
  Mesh m;
  int frame = 0;
  AlloApp() {

    oscRecv().open(PORT, "", 0.016, Socket::UDP);
    oscRecv().handler(*this);
    oscRecv().start();

    addSphere(m);
    initWindow();
    nav().pos(0, 0, 30);
  }

  virtual void onAnimate(double dt) {
  }

  virtual void onMessage(Message& m) {
    if (m.addressPattern() == "/pose") {
      Vec3d& p(nav().pos());
      Quatd& q(nav().quat());
      m >> p.x >> p.y >> p.z >> q[0] >> q[1] >> q[2] >> q[3];
    }
    else if (m.addressPattern() == "/frame") {
      m >> frame;
      cout << frame << endl;
    } else {
      m.print();
    }
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) { g.draw(m); }
};

int main() {
  AlloApp app;
  app.start();
}
