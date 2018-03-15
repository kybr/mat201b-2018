#include "common.h"

#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"

using namespace al;
using namespace std;

struct AlloApp : OmniStereoGraphicsRenderer {
  Light light;
  Mesh bgMesh;
  Texture bgTexture;

  Mesh m;

  AlloApp() {
    // set nav position and light position
    nav().pos(0, 0, 4);
    light.pos(1, 1, 4);

    // set near/far clip
    lens().near(0.1);
    lens().far(1000);  // increase far clip to 1000 GL Units

    // generate sphere with texture coordinates
    addSphereWithTexcoords(bgMesh);

    // generate cube with normals
    addCube(m);
    m.generateNormals();

    // load image into texture print out error and exit if failure
    Image image;
    SearchPaths searchPaths;
    searchPaths.addSearchPath("..");
    string filename = searchPaths.find("worthersee_klagenfurt.jpg").filepath();
    if (image.load(filename)) {
      cout << "Read image from " << filename << endl;
    } else {
      cout << "Failed to read image from " << filename << "!!!" << endl;
      exit(-1);
    }
    bgTexture.allocate(image.array());
  }

  // virtual void onDraw(Graphics& g, const Viewpoint& v) {
  virtual void onDraw(Graphics& g) {
    shader().uniform("lighting", 0.0);
    shader().uniform("texture", 1.0);
    // draw background textured sphere centered at nav
    g.lighting(false);  // turn off lighting
    g.depthMask(
        false);  // disable depth buffer, so that background will be drawn over

    g.pushMatrix();
    g.translate(nav().pos());
    g.rotate(180, 0, 0, 1);
    bgTexture.bind();
    g.color(1, 1, 1);
    g.draw(bgMesh);
    bgTexture.unbind();
    g.popMatrix();

    g.depthMask(true);  // turn depth mask back on
    light();            // enable light
    shader().uniform("lighting", 1.0);
    shader().uniform("texture", 0.0);

    // draw scene here (now just a cube at origin)
    g.pushMatrix();
    g.color(1, 0, 0);
    g.draw(m);
    g.popMatrix();
  }

  virtual void onAnimate(double dt) { pose = nav(); }
};

int main(int argc, char* argv[]) {
  AlloApp app;
  app.start();
  return 0;
}
