// MAT201B
// Original authors: Karl Yerkes and Matt Wright (2012)
// Author: Karl Yerkes (2015)
//

#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"

using namespace al;
using namespace al::osc;
using namespace std;

#include "common.hpp"  // XXX

struct AlloApp : OmniStereoGraphicsRenderer, PacketHandler {
  Recv recv;

  int frame = 0;
  Mesh m;
  AlloApp() {
    recv.open(PORT, "", 0.016, Socket::UDP);
    recv.handler(*this);
    recv.start();

    addSphere(m);
    initWindow();
    pose.pos(0, 0, 10);  // !!! use pose instead of nav
  }

  virtual void onAnimate(double dt) {
    //pose = nav();
  }

  virtual void onDraw(Graphics& g) { g.draw(m); }

  virtual void onMessage(Message& m) {
    if (m.addressPattern() == "/pose") {
      //Vec3d& p(nav().pos());
      //Quatd& q(nav().quat());

      Vec3d& p(pose.pos());
      Quatd& q(pose.quat());
      m >> p.x >> p.y >> p.z >> q[0] >> q[1] >> q[2] >> q[3];
    }
    else if (m.addressPattern() == "/frame") {
      m >> frame;
      cout << frame << endl;
    } else {
      m.print();
    }
  }
};

int main() {
  AlloApp app;
  app.start();
}
