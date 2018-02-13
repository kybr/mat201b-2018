// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
// - Load an image into a texture
// - Bind a texture and draw on a mesh
// - draw textured sphere so that it is always in the background
//

#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

struct AlloApp : App {
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

    initWindow();

    // load image into texture print out error and exit if failure
    Image image;
    SearchPaths searchPaths;
    searchPaths.addSearchPath("../media");
    string filename = searchPaths.find("surround.jpg").filepath();
    if (image.load(filename)) {
      cout << "Read image from " << filename << endl;
    } else {
      cout << "Failed to read image from " << filename << "!!!" << endl;
      exit(-1);
    }
    bgTexture.allocate(image.array());
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
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

    // draw scene here (now just a cube at origin)
    g.pushMatrix();
    g.color(1, 0, 0);
    g.draw(m);
    g.popMatrix();
  }

  virtual void onAnimate(double dt) {}

  // onMouseMove is when the mouse moves
  virtual void onMouseMove(const ViewpointWindow& w, const Mouse& m) {
    // normalize mouse position from -1.0 to 1.0
    float x = float(m.x()) / w.width() * 2.f - 1.f;
    float y = (float(m.y()) / w.height() * 2.f - 1.f) * -1.f;

    // move light with mouse
    light.pos(Vec3f(x, y, 1.f) * 10.f);
  }

  // other mouse callbacks
  //
  virtual void onMouseDown(const ViewpointWindow& w, const Mouse& m) {}
  virtual void onMouseUp(const ViewpointWindow& w, const Mouse& m) {}
  virtual void onMouseDrag(const ViewpointWindow& w, const Mouse& m) {}
};
int main(int argc, char* argv[]) {
  AlloApp app;
  app.start();
  return 0;
}
