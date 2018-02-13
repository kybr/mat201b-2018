// MAT201B
// Original authors: Karl Yerkes and Matt Wright (2012)
// Author: Karl Yerkes (2015)
//

#include "Cuttlebone/Cuttlebone.hpp"  // XXX
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

#include "common.hpp"  // XXX

struct AlloApp : App {
  cuttlebone::Taker<State, CHUNK_SIZE, PORT> taker;  // XXX
  State* state;                                      // XXX

  Mesh m;
  AlloApp() {
    state = new State;
    addSphere(m);
    initWindow();
    nav().pos(0, 0, 30);
  }

  virtual void onAnimate(double dt) {
    taker.get(*state);  // XXX
    nav().set(state->pose);
    state->pose.print();
    cout << state->frame << endl;
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) { g.draw(m); }
};

int main() {
  AlloApp app;
  app.taker.start();  // XXX
  app.start();
}
