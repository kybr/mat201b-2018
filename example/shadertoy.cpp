// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
// - Make a so-called "Shader Toy" or "Live Shader"
//
// - Live coding glsl shader on a screen filling quad, edited shader files are
// reloaded automatically
//

#include "allocore/al_Allocore.hpp"
#include "alloutil/al_ShaderManager.hpp"

using namespace al;
using namespace std;

struct MyApp : App {
  bool inited = false;

  // uniforms
  double time = 0.0;
  Vec2f mousePos;

  ShaderManager sm;

  MyApp() {
    initWindow(Window::Dim(800, 600), "shadertoy");

    // Disable mouse drag to look
    navControl().useMouse(false);
  }

  void loadShaders() {
    // sm.addSearchPath("../shader", true);
    sm.vertLibCode = "#version 120\n";
    sm.addShaderFile("live", "../shader/basic.vert", "../shader/color.frag");
    // sm.addShaderFile("live","basic.vert","waves.frag");
    // sm.addShaderFile("live","basic.vert","tweets.frag");

    inited = true;
  }

  void onAnimate(double dt) { time += dt; }

  void onDraw(Graphics& g) {
    // If there has been a change in the shader files, reload the shaders
    if (!inited || sm.poll()) loadShaders();

    // set projection matrix to orthographic and modelview to origin
    g.projection(Matrix4d::ortho(-1, 1, 1, -1, -1, 1));
    g.modelView(Matrix4d::identity());

    // bind shader program
    ShaderProgram* program = sm.get("live");
    program->begin();

    // set shader uniforms
    program->uniform("time", time);
    program->uniform("mouse", mousePos);

    // draw screen filling quad
    g.begin(g.QUADS);
    g.vertex(-1, -1);
    g.vertex(1, -1);
    g.vertex(1, 1);
    g.vertex(-1, 1);
    g.texCoord(0, 0);
    g.texCoord(1, 0);
    g.texCoord(1, 1);
    g.texCoord(0, 1);
    g.end();

    program->end();
  }

  void onKeyDown(const Keyboard& k) {
    // make keys do things
  }

  // make mouse do things
  virtual void onMouseMove(const ViewpointWindow& w, const Mouse& m) {
    float x = (m.x() * 1. / w.width());
    float y = (m.y() * 1. / w.height());

    mousePos.x = x;
    mousePos.y = y;
  }
  virtual void onMouseDown(const ViewpointWindow& w, const Mouse& m) {}
  virtual void onMouseDrag(const ViewpointWindow& w, const Mouse& m) {}
  virtual void onMouseUp(const ViewpointWindow& w, const Mouse& m) {}
};

int main() {
  MyApp app;
  app.start();
}
