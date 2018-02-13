// MAT201B
// Fall 2015
// Author(s): Tim Wood
//
// Shows how to:
// - Implement "flocking" behavior
//   + See http://www.red3d.com/cwr/boids/
//   + See https://processing.org/examples/flocking.html
//

#include <vector>
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

#define NUM_AGENTS 10
#define TAIL_LENGTH 200
#define WORLD_SIZE 5

struct Agent {
  int id;
  Nav nav;
  Vec3f lastPos;
  HSV hsv;
  Mesh tentacles;
  int idx;

  Agent() {
    tentacles.primitive(Graphics::LINES);
    for (int i = 0; i < TAIL_LENGTH; i++) {
      tentacles.vertex(Vec3f());
      tentacles.color(RGB(1));
    }
    idx = 0;
  }

  void draw(Graphics& g, Mesh& body) {
    g.pushMatrix();
    g.color(hsv);
    g.translate(nav.pos());
    g.rotate(nav.quat());
    g.draw(body);
    g.popMatrix();
    g.draw(tentacles);
  }

  void animate(double dt) {
    // remember previous position and step nav
    lastPos.set(nav.pos());
    nav.step(dt);
    nav.pos().wrap(WORLD_SIZE, -WORLD_SIZE);  // keep within toroidal world

    // animate tail
    int size = tentacles.vertices().size();
    if ((nav.pos() - lastPos).mag() >
        5)  // dont draw line segment if position was wraped
      tentacles.vertices()[idx] = nav.pos();
    else
      tentacles.vertices()[idx] = lastPos;
    idx = wrap(idx + 1, size, 0);
    tentacles.vertices()[idx] = nav.pos();
    idx = wrap(idx + 1, size, 0);

    // set tail colors to fade older segments out
    int c = idx - 1;
    c = wrap(c, size, 0);
    for (int i = 0; i < size; i++) {
      tentacles.colors()[c--] = RGB((size - i) / (float)size);
      c = wrap(c, size, 0);
    }
  }

  // returns vector pointing away from nearby agents
  Vec3f seperate(vector<Agent>& agents) {
    float desiredDist = 1;
    int count = 0;

    Vec3f newDir;

    for (int i = 0; i < agents.size(); i++) {
      if (id == agents[i].id) continue;  // skip self
      Vec3f diff = nav.pos() - agents[i].nav.pos();
      float dist = diff.mag();

      if (dist < desiredDist) {
        newDir += diff.normalize() / dist;
        count++;
      }
    }
    if (count > 0) newDir = newDir / (float)count;
    return newDir;
  }

  // returns average orientation of nearby agents
  Quatf align(vector<Agent>& agents) {
    float maxDist = 2;
    int count = 1;

    Quatf avgQuat(nav.quat());

    for (int i = 0; i < agents.size(); i++) {
      if (id == agents[i].id) continue;  // skip self
      Vec3f diff = nav.pos() - agents[i].nav.pos();
      float dist = diff.mag();

      if (dist < maxDist) {
        count++;
        avgQuat.slerpTo(agents[i].nav.quat(), 0.5);
      }
    }
    return avgQuat;
  }

  // returns center of nearby agents
  Vec3f cohesion(vector<Agent>& agents) {
    float maxDist = 2;
    int count = 0;

    Vec3f avgPos;

    for (int i = 0; i < agents.size(); i++) {
      if (id == agents[i].id) continue;  // skip self
      Vec3f diff = nav.pos() - agents[i].nav.pos();
      float dist = diff.mag();

      if (dist < maxDist) {
        avgPos += agents[i].nav.pos();
        count++;
      }
    }
    if (count > 0) avgPos = avgPos / count;
    return avgPos;
  }
};

struct AlloApp : App {
  Material material;
  Light light;

  Mesh body;
  vector<Agent> agents;

  AlloApp() {
    nav().pos(0, 0, 20);
    light.pos(0, 0, 20);

    addCone(body, 0.1, Vec3f(0, 0, -0.5));

    body.generateNormals();

    for (int i = 0; i < NUM_AGENTS; ++i) {
      Agent a;
      a.id = i;
      a.nav.pos(Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * 5);
      // a.nav.pos(Vec3f(2 * sin(M_PI * 2 * i / N), 2 * cos(M_PI * 2 * i / N),
      // 0));
      a.nav.faceToward(Vec3f(0), 1);
      // a.nav.faceToward(Vec3f(rnd::uniform(),rnd::uniform(),rnd::uniform()),
      // 1);
      a.nav.move(0, 0, 2);
      agents.push_back(a);
    }

    initWindow();
  }

  void addAgent() {
    Agent a;
    a.id = agents.size();
    a.nav.pos(Vec3f());
    Vec3f dir(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
    a.nav.quat(Quatf::getRotationTo(Vec3f(0, 0, 1), dir));
    a.nav.move(0, 0, 2);
    // a.nav.move(0, 0, 1+2*rnd::uniform());
    agents.push_back(a);
  }

  virtual void onAnimate(double dt) {
    for (int a = 0; a < agents.size(); a++) {
      Agent& x = agents[a];

      // calculate seperation vector and rotate towards it
      Vec3f sep = x.seperate(agents);
      if (sep.mag() > 0) x.nav.faceToward(x.nav.pos() + sep, 0.01);

      // calculate alignment orientation and rotate towards it
      Quatf dir = x.align(agents);
      x.nav.quat().slerpTo(dir, 0.01);

      // calculate cohesion center and rotate towards it
      // set color based on distance from crowd center
      Vec3f center = x.cohesion(agents);
      if (center.mag() > 0) {
        x.nav.faceToward(center, 0.01);
        float dist = (center - x.nav.pos()).mag();
        float hue = wrap(dist / 3, 1.0f, 0.0f);
        x.hsv = HSV(hue, 1, 1);
      } else
        x.hsv = HSV(0, 1, 1);
    }

    // animate agents
    for (int i = 0; i < agents.size(); i++) {
      agents[i].animate(dt);
    }
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    material();
    light();
    for (int i = 0; i < agents.size(); ++i) {
      agents[i].draw(g, body);
    }
  }

  virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    switch (k.key()) {
      case ' ':
        addAgent();
        break;  // space bar to add an agent
      case 'r':
        for (int i = 0; i < agents.size(); i++) {
          agents[i].nav.pos(
              Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * 5);
          agents[i].nav.faceToward(Vec3f(0), 1);
        }
        break;
    }
  }
};

int main() { AlloApp().start(); }
