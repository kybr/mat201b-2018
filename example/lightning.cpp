// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
// - Make decent lightning bolts
//
// http://gamedevelopment.tutsplus.com/tutorials/how-to-generate-shockingly-good-2d-lightning-effects--gamedev-2681
//
// for more realistic lightning see
// http://gamma.cs.unc.edu/FAST_LIGHTNING/lightning_tvcg_2007.pdf
//

#include <algorithm>
#include <vector>
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

struct AlloApp : App {
  Texture texture;
  Mesh mesh;
  Color color;
  double time;
  double strikeTime;

  AlloApp() {
    // using GL_TRIANGLES so that we can have branching lighting in the same
    // mesh
    mesh.primitive(Graphics::TRIANGLES);

    // animation variables
    time = 0;
    strikeTime = 1.0;

    // texture we will write our lightning sprite into
    // it only needs to be 1 pixel high because it doesn't change over y
    // and will look seemless when it is repeated next to itself
    texture = Texture(256, 1, Graphics::LUMINANCE_ALPHA, Graphics::UBYTE, true);
    Array& sprite(texture.array());

    struct {
      uint8_t l, a;
    } lum;

    for (size_t col = 0; col < sprite.width(); ++col) {
      float x = float(col) / (sprite.width() - 1) * 2 - 1;

      // draw lightning, white in the center, and fades out
      // fade out slower near the edges
      if (abs(x) < 0.2)
        lum.l = mapRange(abs(x), 0.f, .2f, 255.f, 60.f);
      else
        lum.l = mapRange(abs(x), 0.2f, 1.f, 60.f, 0.f);
      lum.a = lum.l;
      sprite.write(&lum.l, col, 0);
    }

    // set nav position and disable mouse drag to look
    nav().pos(0, 0, 6);
    navControl().useMouse(false);

    initWindow();
    initAudio();
  }

  virtual void onAnimate(double dt) {
    // accumulate time steps (dt) into our time variable.
    time += dt;

    // generate new lightning bolt if elapsed time greater than strike time
    if (time > strikeTime) {
      // clear the lightning mesh
      mesh.reset();

      // choose random x location for lighting bolt and generate bolt
      float x = rnd::uniformS(2);
      bolt(Vec3f(x, 1, 0), Vec3f(x, -1, 0), 4, 0.02);

      // choose random time for next strike
      time = 0;
      strikeTime = rnd::uniform(3.0);
    }

    color *= 0.95;  // fade lighting out
  }

  // generate a bolt of lightning and add to our mesh
  // modified from Michael Hoffman's
  // (http://gamedevelopment.tutsplus.com/tutorials/how-to-generate-shockingly-good-2d-lightning-effects--gamedev-2681)
  void bolt(Vec3f source, Vec3f dest, int maxBranches = 0,
            float branchProb = 0.01, float wid = 0, int n = 100) {
    Vec3f tangent = (dest - source);  // direction of lightning
    Vec3f normal =
        tangent.cross(Vec3f(0, 0, -1)).normalize();  // normal to lightning
    float length = tangent.mag();

    // choose n random positions (0,1) along lightning vector and sort them
    vector<float> positions;
    positions.push_back(0);
    for (int i = 0; i < n; i++) positions.push_back(rnd::uniform());
    sort(positions.begin(), positions.end());

    float sway = 1;  // max random walk step of displacement along normal
    float jaggedness = 1 / sway;

    Vec3f prevPoint = source;
    float prevDisplacement = 0;
    float width = (wid > 0) ? wid : length / 20.0 + 0.01;

    int branches = 0;

    for (int i = 1; i < n; i++) {
      float pos = positions[i];

      // used to prevent sharp angles by ensuring very close positions also have
      // small perpendicular variation.
      float scale = (length * jaggedness) * (pos - positions[i - 1]);

      // defines an envelope. Points near the middle of the bolt can be further
      // from the central line.
      float envelope = pos > 0.95f ? mapRange(pos, 0.95f, 1.0f, 1.0f, 0.0f) : 1;

      // displacement from prevDisplacement (random walk (brownian motion))
      float displacement = rnd::uniformS(sway) * scale + prevDisplacement;
      displacement *= envelope;

      // calculate point, source plus percentage along tangent, and displacement
      // along normal;
      Vec3f point = source + pos * tangent + displacement * normal;

      // generate triangles (vertices and texture coordinates) from point and
      // prevPoint
      mesh.vertex(prevPoint + normal * width);
      mesh.vertex(prevPoint - normal * width);
      mesh.vertex(point + normal * width);
      mesh.vertex(point + normal * width);
      mesh.vertex(prevPoint - normal * width);
      mesh.vertex(point - normal * width);

      // our texture doesn't change in y so we can ignore the second texCoord
      mesh.texCoord(0, 0);
      mesh.texCoord(1, 0);
      mesh.texCoord(0, 0);
      mesh.texCoord(0, 0);
      mesh.texCoord(1, 0);
      mesh.texCoord(1, 0);

      // recursively generate lightning branches
      if (branches < maxBranches && rnd::prob(branchProb)) {
        branches++;
        Vec3f dir(tangent);
        rotate(dir, Vec3f(0, 0, 1), rnd::uniformS(30) * M_DEG2RAD);
        dir.normalize();
        float len = (dest - point).mag();
        bolt(point, point + dir * len, maxBranches - 1, branchProb, width * 0.8,
             n);
      }

      // remember previous point and displacement for next iteration
      prevPoint = point;
      prevDisplacement = displacement;
    }
    color = Color(1, 0.85, 1, 1);  // make bright and purpleish
  }

  virtual void onMouseDown(const ViewpointWindow& w, const Mouse& m) {
    Rayd r = getPickRay(w, m.x(), m.y());
    float t = r.intersectPlane(Vec3f(), (Vec3f(0, 0, 1)));
    Vec3f dest = r(t);
    Vec3f src;
    mesh.reset();
    bolt(src, dest);
  }
  virtual void onMouseDrag(const ViewpointWindow& w, const Mouse& m) {
    Rayd r = getPickRay(w, m.x(), m.y());
    float t = r.intersectPlane(Vec3f(), (Vec3f(0, 0, 1)));
    Vec3f dest = r(t);
    Vec3f src;
    bolt(src, dest);
  }

  virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    strikeTime = 0;
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    // turn on additive blending
    g.depthTesting(false);
    g.blending(true);
    // g.blendModeTrans();
    g.blendMode(g.ONE, g.ONE);

    // draw our mesh with texture sprites enabled and our texture "bound".
    texture.bind();
    g.color(color);  // color used to fade out mesh
    g.draw(mesh);
    texture.unbind();
  }

  // ADD THUNDER HERE!!!!!!!!
  virtual void onSound(AudioIOData& io) {
    // gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      float s = 0;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() { AlloApp().start(); }
