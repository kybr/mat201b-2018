// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
// - Load a font
// - Render text in 2d and 3d scene
// - Use Quatd::getBillboardRotation to make billboarded text
//
//
// Based on Allocore Example font.cpp: Rendering text with Font in 2D
// by Lance Putnam, Sept. 2013

#include "allocore/graphics/al_Font.hpp"
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

class MyApp : public App {
 public:
  Font font1;
  Mesh m;
  Light light;
  string msg1 = "I AM A BANANA!";
  string msg2 = "no.. you really aren't.";
  string s1, s2;
  double stateTime = 0, timer = 0;
  int state = 0;

  MyApp() : font1(fullPathOrDie("Avenir-Medium.otf", ".."), 40) {
    nav().pos(0, 0, 10);
    light.pos(0, 0, 10);
    addSphere(m);
    m.generateNormals();
    initWindow();
  }

  void onAnimate(double dt) {
    stateTime += dt;
    timer += dt;
    // if state 0, after 1 seconds and every 0.25 seconds
    if ((state == 0) && (stateTime > 1.0) && (timer > 0.25)) {
      if (s1.length() < msg1.length()) {
        s1 += msg1[s1.length()];  // add character of msg1 to s1
      } else {
        state = 1;
        stateTime = 0;
      }
      timer = 0;  // reset timer
    }

    // if state 1, after 3 seconds, every 0.1 seconds
    if ((state == 1) && (stateTime > 3.0) && timer > 0.1) {
      if (s2.length() < msg2.length()) {
        s2 += msg2[s2.length()];  // add character of msg2 to s2
      } else {
        state = 2;
        stateTime = 0;
      }
      timer = 0;  // reset timer
    }

    if (state == 2 && stateTime > 4.0) {
      state = 3;
      stateTime = 0;
      s1 = "";
      s2 = "";
      msg1 = "@*&$!";
      msg2 = "OH MY!";
    }

    // if state 3, after 2 seconds, every 0.1 seconds
    if ((state == 3) && (stateTime > 2.0) && timer > 0.1) {
      if (s1.length() < msg1.length()) {
        s1 += msg1[s1.length()];  // add character of msg1 to s1
      } else {
        state = 4;
        stateTime = 0;
      }
      timer = 0;  // reset timer
    }

    // if state 4, after 1 seconds, every 0.1 seconds
    if ((state == 4) && (stateTime > 1.0) && timer > 0.1) {
      if (s2.length() < msg2.length()) {
        s2 += msg2[s2.length()];  // add character of msg2 to s2
      } else {
        state = 5;
        stateTime = 0;
      }
      timer = 0;  // reset timer
    }
  }

  void onDraw(Graphics& g, const Viewpoint& v) {
    /*
     * Text Billobarded in 3D
     */

    light();

    // draw sphere and text
    g.lighting(true);
    g.pushMatrix();
    g.color(1, 1, 0);
    g.translate(-2, 0, 0);
    g.draw(m);                  // draw mesh
    g.translate(-0.9, 1.1, 0);  // move text over sphere
    g.scale(0.007);             // scale down since the text is 40 units high
    g.blendAdd();         // additive blending to ignore text's black background
    g.lighting(false);    // turn light off for text
    font1.render(g, s1);  // draw text
    g.blendOff();
    g.popMatrix();

    // same for other sphere
    g.lighting(true);
    g.pushMatrix();
    g.color(1, 0, 1);
    g.translate(2, 0, 0);
    g.draw(m);
    g.translate(-0.75, 1.1, 0);
    g.scale(0.005);

    // except lets rotate this text to always face the camera and stay upright
    Vec3d textPos = Vec3d(2.0 - 0.75, 1.1, 0);  // sum of translations..
    Vec3d forward = Vec3d(nav().pos() - textPos)
                        .normalize();  // vector from source to destination
    Quatd rot = Quatd::getBillboardRotation(
        forward, nav().uu());  // pass forward vector and up vector
    g.rotate(rot);

    g.blendAdd();
    g.lighting(false);
    font1.render(g, s2);
    g.blendOff();
    g.popMatrix();

    /*
     * 2D Text
     */
    // Get the viewport dimensions, in pixels, for positioning the text
    float W = v.viewport().w;
    float H = v.viewport().h;

    // Setup our matrices for 2D pixel space
    g.pushMatrix(Graphics::PROJECTION);
    g.loadMatrix(Matrix4f::ortho2D(0, W, 0, H));
    g.pushMatrix(Graphics::MODELVIEW);

    // Before rendering text, we must turn on blending
    g.blendAdd();

    // Render text in the top-left corner
    g.loadIdentity();
    g.translate(8, H - (font1.size() + 8));
    g.currentColor(1, 1, 0, 1);
    font1.render(g, "Top-left 2d text");

    // Render text in the bottom-left corner
    g.loadIdentity();
    g.translate(8, 8);
    g.currentColor(1, 0, 1, 1);
    font1.render(g, "Bottom-left 2d text");

    // Render text centered on the screen
    g.loadIdentity();
    std::string str = "Centered 2d text";
    // Note that dimensions must be integers to avoid blurred text
    g.translate(int(W / 2 - font1.width(str) / 2),
                int(H / 2 - font1.size() / 2));
    g.currentColor(0, 1, 1, 1);
    font1.render(g, str);

    // Turn off blending
    g.blendOff();

    g.popMatrix();
    g.popMatrix(Graphics::PROJECTION);
  }

  void onKeyDown(const Keyboard& k) {
    state = 0;
    stateTime = 0;
    timer = 0;
    s1 = "";
    s2 = "";
    msg1 = "I AM A BANANANAAAANA!";
    msg2 = "sorry... but... no.";
  }
};

int main() { MyApp().start(); }
