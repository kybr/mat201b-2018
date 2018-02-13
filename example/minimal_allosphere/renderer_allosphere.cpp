// MAT201B
// Original authors: Karl Yerkes and Matt Wright (2012)
// Author: Karl Yerkes (2015)
//

#include "Cuttlebone/Cuttlebone.hpp"  // XXX
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"

using namespace al;
using namespace std;

#include "common.hpp"  // XXX

struct AlloApp : OmniStereoGraphicsRenderer {
  cuttlebone::Taker<State, CHUNK_SIZE, PORT> taker;  // XXX
  State* state;                                      // XXX

  Mesh m;
  AlloApp() {
    state = new State;
    addSphere(m);
    initWindow();
    pose.pos(0, 0, 10);  // !!! use pose instead of nav
  }

  virtual void onAnimate(double dt) {
    taker.get(*state);  // XXX
    pose = state->pose;
    state->pose.print();
    cout << state->frame << endl;
  }

  virtual void onDraw(Graphics& g) { g.draw(m); }
};

int main() {
  AlloApp app;
  app.taker.start();  // XXX
  app.start();
}
