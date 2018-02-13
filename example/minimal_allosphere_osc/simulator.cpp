// MAT201B
// Original authors: Karl Yerkes and Matt Wright (2012)
// Author: Karl Yerkes (2015)

#include "allocore/io/al_App.hpp"
#include "alloutil/al_Simulator.hpp"  // automatic IP address

using namespace al;
using namespace al::osc;
using namespace std;

#include "common.hpp"  // XXX

struct AlloApp : App, InterfaceServerClient {  // XXX
  Mesh m;

  AlloApp()
      : InterfaceServerClient(Simulator::defaultInterfaceServerIP())  // XXX
  {
    // configure the port and ip of our UDP message sender
    App::oscSend().open(PORT, Simulator::defaultBroadcastIP(), 0.016,
                        Socket::UDP);

    addSphere(m);

    initWindow();
    initAudio();

    // set interface server nav/lens to App's nav/lens
    //
    InterfaceServerClient::setNav(nav());    // XXX
    InterfaceServerClient::setLens(lens());  // XXX
    nav().pos(0, 0, 30);
  }

  int frame = 0;

  virtual void onAnimate(double dt) {
    // handle messages from interface server
    //
    while (InterfaceServerClient::oscRecv().recv())
      ;  // XXX

    Vec3d& p = nav().pos();
    Quatd& q = nav().quat();
    App::oscSend().send("/pose", p.x, p.y, p.z, q[0], q[1], q[2], q[3]);
    App::oscSend().send("/frame", frame++);
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) { g.draw(m); }

  // TODO: add audioscene here..
  virtual void onSound(AudioIOData& io) {
    while (io()) {
      float s = 0;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() {
  AlloApp app;
  app.start();
}
