// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
// - Spawn a thread
//

#include <iostream>
#include <unistd.h>
#include <thread>
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

struct MyApp : App {
  // thread stuff
  thread computeThread;
  bool done;

  Mesh m;

  MyApp() {
    initWindow(Window::Dim(0, 0, 600, 400), "myApp", 60);
    m.primitive(Graphics::LINES);
    m.vertex(Vec3f(0, 0, 0));
    m.vertex(Vec3f(0, 0.1, 0));
    nav().pos(0, 0, 2);
  }

  // Do your computation in this function, it's called by our thread spawned in
  // startThread.
  // This is something that takes time and would cause onAnimate to drop frames
  void doComputation() {
    static int lastSize = 0;

    sleep(1);  // simulate some long computation by sleeping

    // add lines connecting vertices in a random direction
    int size = m.vertices().size();
    for (int i = 0; i < 20; i++) {
      int idx = rnd::uniform(size, lastSize);
      Vec3f old = m.vertices()[idx];
      Vec3f dir;
      if (rnd::prob(0.9))
        dir = (old +
               Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * 0.2)
                  .normalize();
      else
        dir = Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS())
                  .normalize();
      m.vertex(old);
      m.vertex(old + dir * 0.01);
    }
    lastSize = size;
  }

  void startThread() {
    // start compute thread
    done = false;
    computeThread = thread([&]() {
      int cycle = 0;
      while (!done) {
        cout << "compute cycle " << cycle << endl;
        cycle++;
        doComputation();
      }
    });
  }

  void stopThread() {
    done = true;
    computeThread.join();
  }

  virtual void onAnimate(double dt) {}

  virtual void onDraw(Graphics& g, const Viewpoint& v) { g.draw(m); }

  virtual void onSound(AudioIOData& io) {}

  virtual void onKeyDown(const ViewpointWindow& w, const Keyboard& k) {}

  virtual void onMouseDown(const ViewpointWindow& w, const Mouse& m) {}
};

int main() {
  MyApp app;
  app.startThread();
  app.start();
}
