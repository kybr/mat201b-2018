// MAT 201B
// Winter 2017
// Author: Karl Yerkes
//
// Minimal OSC listener
//
#include "allocore/al_AlloCore.hpp"

using namespace al;
using namespace al::osc;
using namespace std;

struct MyApp : PacketHandler, App {
  MyApp() {
    oscRecv().open(60777, "", 0.016, Socket::UDP);
    oscRecv().handler(*this);
    oscRecv().start();
  }

  virtual void onMessage(Message& m) {
    m.print();

    if (m.addressPattern() == "/frame") {
      int frame;
      m >> frame;
      cout << "got frame: " << frame << endl;
    } else if (m.addressPattern() == "/key") {
      int key;
      m >> key;
      cout << "got key: " << key << endl;
    } else {
      m.print();
    }
  }
};

int main() { MyApp().start(); }
