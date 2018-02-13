// MAT201B
// Original authors: Karl Yerkes and Matt Wright (2012)
// Author: Karl Yerkes (2015)

#include "Cuttlebone/Cuttlebone.hpp"  // XXX
#include "allocore/io/al_App.hpp"
#include "alloutil/al_Simulator.hpp"  // XXX

using namespace al;
using namespace std;

#include "common.hpp"  // XXX

void printFactsAboutState(int size) {
  cout << "==================================================" << endl;
  cout << "Your state type takes " << size << " bytes in memory." << endl;

  if (size > 1024.0f * 1024.0f * 1024.0f) {
    cout << "That is " << size / (1024.0f * 1024.0f * 1024.0f) << " gigabytes."
         << endl;
    cout << "That's too big." << endl;
  } else if (size > 1024.0f * 1024.0f * 10) {
    cout << "That is " << size / (1024.0f * 1024.0f) << " megabytes." << endl;
    cout << "That's a very big state." << endl;
  } else if (size > 1024.0f * 1024.0f) {
    cout << "That is " << size / (1024.0f * 1024.0f) << " megabytes." << endl;
  } else if (size > 1024.0f) {
    cout << "That is " << size / 1024.0f << " kilobytes." << endl;
  } else {
  }

  // Standard 1Gb Ethernet effective bandwidth
  //
  float gbe = 1.18e+8f;

  cout << "On a 1Gb Ethernet LAN (like the MAT network), ";
  if (gbe / size > 60.0f)
    cout << "you will use \n"
         << 100.0f * (size * 60.0f) / gbe
         << "\% of the effective bandwidth with a framerate of 60 Hz." << endl;
  else
    cout << "your framerate will be *network limited* to " << gbe / size
         << " Hz." << endl;

  cout << "On a 10Gb Ethernet LAN (like the AlloSphere network), ";
  if (10 * gbe / size > 60.0f)
    cout << "you will use \n"
         << 100.0f * (size * 60.0f) / (10 * gbe)
         << "\% of the effective bandwidth with a framerate of 60 Hz." << endl;
  else {
    cout << "your framerate will be *network limited* to " << 10 * gbe / size
         << " Hz." << endl;
    cout << "Your state is very large." << endl;
  }
  cout << "==================================================" << endl;
}

struct AlloApp : App, InterfaceServerClient {  // XXX
  Mesh m;

  cuttlebone::Maker<State, CHUNK_SIZE, PORT> maker;  // XXX
  State* state;                                      // XXX

  AlloApp()
      : maker(Simulator::defaultBroadcastIP()),                       // XXX
        InterfaceServerClient(Simulator::defaultInterfaceServerIP())  // XXX
  {
    state = new State;

    printFactsAboutState(sizeof(State));

    state->frame = 0;  // XXX

    addSphere(m);

    initWindow();
    initAudio();

    // set interface server nav/lens to App's nav/lens
    //
    InterfaceServerClient::setNav(nav());    // XXX
    InterfaceServerClient::setLens(lens());  // XXX
    nav().pos(0, 0, 30);
  }

  virtual void onAnimate(double dt) {
    // handle messages from interface server
    //
    while (InterfaceServerClient::oscRecv().recv())
      ;  // XXX

    state->pose = nav();
    maker.set(*state);  // XXX
    state->frame++;     // XXX
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
  app.maker.start();  // XXX
  app.start();
}
