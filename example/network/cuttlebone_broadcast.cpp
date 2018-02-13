// MAT 201B
// Winter 2017
// Author: Karl Yerkes
//
// Minimal cuttlebone broadcaster
//
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/al_AlloCore.hpp"
#include "cuttlebone_common.hpp"

using namespace al;
using namespace al::osc;
using namespace std;

struct MyApp : App {
  cuttlebone::Maker<State> maker;
  State* state = new State;

  MyApp() : maker("255.255.255.255") {
    // MyApp() : maker("127.0.0.1") {
    state->frame = 0;
    state->key = 0;
    state->showFrame = false;

    initWindow();
  }

  void onAnimate(double dt) {
    maker.set(*state);
    state->frame++;
  }

  virtual void onKeyDown(const Keyboard& k) {
    if (k.key() == ' ') {
      state->showFrame = !state->showFrame;
    }
    state->key = k.key();
  }
};

int main() {
  MyApp app;
  app.maker.start();
  app.start();
}
