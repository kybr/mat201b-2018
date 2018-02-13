// MAT 201B
// Winter 2017
// Author: Karl Yerkes
//
// Minimal cuttlebone listener
//
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/al_AlloCore.hpp"
#include "cuttlebone_common.hpp"

using namespace al;
using namespace al::osc;
using namespace std;

struct MyApp : App {
  cuttlebone::Taker<State> taker;
  State* state = new State;

  // for remembering the last key pressed on the master
  int lastKey = 0;

  MyApp() { initWindow(); }

  void onAnimate(double dt) {
    int popCount = taker.get(*state);
    if (popCount < 1) {
      // if we didn't get any states, then just skip any output
      return;
    }

    if (state->showFrame) {
      cout << "got frame: " << state->frame << endl;
    }
    if (state->key != lastKey) {
      cout << "got key: " << state->key << endl;
      lastKey = state->key;
    }
  }
};

int main() {
  MyApp app;
  app.taker.start();
  app.start();
}
