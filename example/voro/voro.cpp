// Single Voronoi cell example code
//
// Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

#include "allocore/al_Allocore.hpp"
#include "voro++/voro++.hh"
using namespace voro;
using namespace al;

// N is the number of planes we cut the voronoi cell with
#define N (1000)

// reinterpret a voronoicell as an al::Mesh
//
// adapted from openFrameworks' ofxVoro addon code found here:
//
//   https://github.com/patriciogonzalezvivo/ofxVoro/blob/master/src/ofxVoro.cpp
//
void addVoroWireframe(Mesh& m, voronoicell& v) {
  m.primitive(Graphics::LINES);

  double* raw = v.pts;
  for (int i = 0; i < v.p; i++, raw += 3) {
    Vec3f point;
    point.x = raw[0];  // * 0.5;
    point.y = raw[1];  // * 0.5;
    point.z = raw[2];  // * 0.5;
    m.vertex(point);
  }

  for (int i = 1; i < v.p; i++) {
    for (int j = 0; j < v.nu[i]; j++) {
      int k = v.ed[i][j];

      if (k >= 0) {
        m.index(i);
        m.index(k);
      }
    }
  }
}

struct Appp : App {
  Mesh voronoi;

  Appp() {
    double x, y, z, rsq, r;
    voronoicell v;

    // Initialize the Voronoi cell to be a cube of side length 2, centered
    // on the origin
    v.init(-1, 1, -1, 1, -1, 1);

    // Cut the cell by N random planes which are all a distance 1 away
    // from the origin, to make an approximation to a sphere
    for (int i = 0; i < N; i++) {
      x = 2 * rnd::uniform() - 1;
      y = 2 * rnd::uniform() - 1;
      z = 2 * rnd::uniform() - 1;
      rsq = x * x + y * y + z * z;
      if (rsq > 0.01 && rsq < 1) {
        r = 1 / sqrt(rsq);
        x *= r;
        y *= r;
        z *= r;
        v.plane(x, y, z, 1);
      }
    }

    // add the voronoicell to our mesh
    // (see function definition)
    addVoroWireframe(voronoi, v);

    initWindow();
  }

  void onDraw(Graphics& g) {
    g.clearColor(0.15, 0.15, 0.15, 1);
    g.clear(Graphics::COLOR_BUFFER_BIT);
    g.draw(voronoi);
  }
};

int main() { Appp().start(); }
