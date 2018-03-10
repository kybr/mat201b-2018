#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

const float sphereRadius = 1;
const int particleCount = 30;
const float scaleFactor = 0.3;

Mesh sphere;  // global prototype; leave this alone

// helper function: makes a random vector
Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }

struct Particle {
  Vec3f position, velocity;
  float speed, phase = 0;
  Color c;
  Particle() {
    position = r().normalize() * 23;
    velocity = r().normalize();
    speed = rnd::uniform(0.005, 0.06);
    c = HSV(rnd::uniform(), 0.7, 1);
  }
  void step() {
    Quatf q;
    q.fromAxisAngle(speed, velocity);
    q.normalize();
    position = q.rotate(position);

    phase += speed;
  }
  void draw(Graphics& g) {
    g.pushMatrix();
    g.translate(position);
    g.color(c);
    g.draw(sphere);
    g.popMatrix();
  }
};

struct MyApp : App {
  Material material;
  Light light;
  vector<Particle> particle;
  Particle special;

  MyApp() {
    addSphere(sphere, sphereRadius);
    sphere.generateNormals();
    light.pos(0, 0, 0);              // place the light
    nav().pos(0, 0, 30);             // place the viewer
    particle.resize(particleCount);  // make all the particles
    special.c = Color(1);
    background(Color(0.07));
    lens().far(200);
    initWindow();
    initAudio();
  }

  void onAnimate(double dt) {
    static double t = 0;
    static int which = 0;
    t += dt;
    if (t > 3) {
      t -= 3;
      which = rnd::uniform(particleCount);
    }

    Quatf p(particle[which].position), q(special.position);
    p.normalize();
    q.normalize();
    q.slerpTo(p, 0.03);
    special.position.x = q.x;
    special.position.y = q.y;
    special.position.z = q.z;
    special.position.normalize(23);

    for (auto& p : particle) p.step();
  }

  void onDraw(Graphics& g) {
    material();
    light();
    g.scale(scaleFactor);
    special.draw(g);
    for (auto p : particle) p.draw(g);
  }

  void onSound(AudioIO& io) {
    while (io()) {
      io.out(0) = 0;
      io.out(1) = 0;
    }
  }

  void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    switch (k.key()) {
      default:
        break;
    }
  }
};

int main() {
  MyApp app;
  app.start();
}
