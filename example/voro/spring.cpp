// Single Voronoi cell example code
//
// Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

#include <fstream>
#include <set>
#include "allocore/al_Allocore.hpp"
#include "voro++/voro++.hh"
using namespace voro;
using namespace al;
using namespace std;

const float stiffness = 10.0;
const float damping = 0.97;
const float timeStep = 0.1;
const float tightness = 0.6666;

void save(ofstream& out, Mesh& m) {
  unsigned int V(m.vertices().size()), I(m.indices().size());

  out.write((char*)&V, sizeof(int));
  out.write((char*)&I, sizeof(int));

  for (int v = 0; v < V; v++)
    out.write((char*)m.vertices()[v].elems(), sizeof(Vec3f));

  for (int i = 0; i < I; i++)
    out.write((char*)&m.indices()[i], sizeof(unsigned int));
}

void load(ifstream& in, Mesh& m) {
  m.primitive(Graphics::LINES);

  unsigned V, I;
  in.read((char*)&V, sizeof(unsigned int));
  in.read((char*)&I, sizeof(unsigned int));

  Vec3f p;
  for (int v = 0; v < V; v++) {
    in.read((char*)p.elems(), sizeof(Vec3f));
    m.vertex(p);
  }

  unsigned int t;
  for (int i = 0; i < I; i++) {
    in.read((char*)&t, sizeof(unsigned int));
    m.index(t);
  }
}

void make(voronoicell& v) {
  // make a *unique* random number generator
  set<float> s;
  auto uniq = [&] {
    float r = rnd::uniformS();
    while (s.count(r)) {
      r = rnd::uniformS();
    }
    return r;
  };

  v.init(-1, 1, -1, 1, -1, 1);
  while (v.p < 100000) {
    Vec3f p(uniq(), uniq(), uniq());
    p.normalize();
    v.plane(p.x, p.y, p.z, 1);
  }

  for (int i = 0; i < v.p; i++)
    if (v.nu[i] != 3) exit(42);
}

void addVoroWireframe(Mesh& m, voronoicell& v) {
  m.primitive(Graphics::LINES);

  double* raw = v.pts;
  for (int i = 0; i < v.p; i++, raw += 3)
    m.vertex(Vec3f(raw[0], raw[1], raw[2]));

  set<uint64_t> longset;
  for (int i = 0; i < v.p; i++) {
    // for (int i = 1; i < v.p; i++) {
    for (int j = 0; j < 3; j++) {
      int k = v.ed[i][j];
      if (k < 0) continue;

      uint64_t a = i, b = k;
      uint64_t q = (a << 32) + b, p = a + (b << 32);
      if (longset.count(q))
        continue;
      else
        longset.insert(q);
      if (longset.count(p))
        continue;
      else
        longset.insert(p);

      m.index(i);
      m.index(k);
    }
  }
}

struct Spring {
  unsigned int a, b;
  float restingLength;
  Vec3f force;
};

struct Appp : App {
  Mesh voronoi, particle;
  vector<Spring> spring;
  vector<Vec3f> velocity;

  int mode = 1;

  Appp() {
    ifstream in("mesh.bin", ios::in | ios::binary);
    if (in.is_open()) {
      load(in, voronoi);
      cout << "We loaded our mesh from the cache." << endl;
    } else {
      cout << "We're making a new mesh and caching it." << endl;
      voronoicell v;
      make(v);
      addVoroWireframe(voronoi, v);
      ofstream out("mesh.bin", ios::out | ios::binary);
      save(out, voronoi);
    }

    for (int i = 0; i < voronoi.indices().size(); i += 2)
      spring.push_back({voronoi.indices()[i], voronoi.indices()[i + 1],
                        tightness *
                            (voronoi.vertices()[voronoi.indices()[i]] -
                             voronoi.vertices()[voronoi.indices()[i + 1]])
                                .mag(),
                        0.0f});

    cout << voronoi.vertices().size() << endl;
    cout << voronoi.indices().size() << endl;
    cout << spring.size() << endl;

    velocity.resize(voronoi.vertices().size(), Vec3f(0, 0, 0));

    initWindow();
  }

  void onAnimate(double dt) {
    switch (mode) {
      case 0:
        distribute(dt);
        break;
      case 1:
        springStep(dt);
        break;
      case 2:
        break;
    }
  }

  void distribute(double dt) {}

  void springStep(double dt) {
    for (auto& s : spring) {
      Vec3f& a(voronoi.vertices()[s.a]);
      Vec3f& b(voronoi.vertices()[s.b]);
      Vec3f diff(a - b);
      float mag = diff.mag();
      s.force = diff * (mag - s.restingLength) * -stiffness;
    }

    for (int i = 0; i < velocity.size(); i++)
      velocity[i] +=
          (voronoi.vertices()[i].normalize() - voronoi.vertices()[i]) *
          stiffness * timeStep;

    for (auto& s : spring) {
      velocity[s.a] += s.force * timeStep;
      velocity[s.b] -= s.force * timeStep;
    }

    for (int i = 0; i < velocity.size(); i++) {
      velocity[i] *= damping;
    }

    for (int i = 0; i < velocity.size(); i++) {
      voronoi.vertices()[i] += velocity[i] * timeStep;
    }
  }

  void onKeyDown(const Keyboard& k) {
    voronoi.vertices()[0] +=
        Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * 0.1;

    if (k.isNumber()) mode = k.keyAsNumber();
  }

  void onDraw(Graphics& g) {
    g.clearColor(0.15, 0.15, 0.15, 1);
    g.clear(Graphics::COLOR_BUFFER_BIT);

    switch (mode) {
      case 0:
        g.draw(particle);
        break;
      case 1:
        g.draw(voronoi);
        break;
      case 2:
        break;
    }
  }
};

int main() { Appp().start(); }
