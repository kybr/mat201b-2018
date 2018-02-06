#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

// some of these must be carefully balanced; i spent some time turning them.
// change them however you like, but make a note of these settings.
unsigned particleCount = 2;       // try 2, 5, 50, and 5000
float maximumAcceleration = 1e6;  // prevents explosion, loss of particles
float initialRadius = 30;         // initial condition
float initialSpeed = 0;           // initial condition
float gravityFactor = 1e5;        // see Gravitational Constant
float springFactor = 100;         //
float dragFactor = 0.1;           //
float timeStep = 0.015625;        // keys change this value for effect
float scaleFactor = 0.1;          // resizes the entire scene
float sphereRadius = 20;  // increase this to make collisions more frequent

Mesh sphere;  // global prototype; leave this alone

// helper function: makes a random vector
Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }

struct Particle {
  Vec3f position, velocity, acceleration;
  Color c;
  Particle() {
    position = r() * initialRadius;
    velocity =
        // this will tend to spin stuff around the y axis
        Vec3f(0, 1, 0).cross(position).normalize(initialSpeed);
    c = HSV(rnd::uniform(), 0.7, 1);
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
  bool simulate = true;
  unsigned frameCount = 0;
  vector<Particle> particle;

  MyApp() {
    addSphere(sphere, sphereRadius);
    sphere.generateNormals();
    light.pos(0, 0, 0);              // place the light
    nav().pos(0, 0, 30);             // place the viewer
    lens().far(400);                 // set the far clipping plane
    particle.resize(particleCount);  // make all the particles
    background(Color(0.07));

    initWindow();
    initAudio();
  }

  void onAnimate(double dt) {
    if (!simulate)
      // skip the rest of this function
      return;

    //
    //  Detect Collisions Here
    //
    unsigned collisionCount = 0;
    for (unsigned i = 0; i < particle.size(); ++i)
      for (unsigned j = 1 + i; j < particle.size(); ++j) {
        Particle& a = particle[i];
        Particle& b = particle[j];

        Vec3f difference = (b.position - a.position);
        float d = difference.mag();
        if (d > 2 * sphereRadius) continue;
        float compressionFactor = 2 * sphereRadius - d;
        Vec3f acceleration =
            difference.normalize(compressionFactor * -springFactor);
        a.acceleration += acceleration;
        b.acceleration -= acceleration;
        //
        collisionCount++;
      }
    if (collisionCount)
      printf("%u: %u collisions\n", frameCount, collisionCount);

    for (unsigned i = 0; i < particle.size(); ++i)
      for (unsigned j = 1 + i; j < particle.size(); ++j) {
        Particle& a = particle[i];
        Particle& b = particle[j];

        Vec3f difference = (b.position - a.position);
        float d = difference.mag();
        Vec3f acceleration = difference / (d * d * d) * gravityFactor;
        a.acceleration += acceleration;
        b.acceleration -= acceleration;
      }

    for (auto& p : particle) p.acceleration += p.velocity * -dragFactor;

    // Limit acceleration
    unsigned limitCount = 0;
    for (auto& p : particle)
      if (p.acceleration.mag() > maximumAcceleration) {
        p.acceleration.normalize(maximumAcceleration);
        limitCount++;
      }
    if (limitCount)
      printf("%u: %u of %u\n", frameCount, limitCount, particle.size());

    // Euler's Method; Keep the time step small
    for (auto& p : particle) p.position += p.velocity * timeStep;
    for (auto& p : particle) p.velocity += p.acceleration * timeStep;
    for (auto& p : particle) p.acceleration.zero();  // XXX zero accelerations

    frameCount++;
  }

  void onDraw(Graphics& g) {
    material();
    light();
    g.scale(scaleFactor);
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
      case '1':
        // reverse time
        timeStep *= -1;
        break;
      case '2':
        // speed up time
        if (timeStep < 1) timeStep *= 2;
        break;
      case '3':
        // slow down time
        if (timeStep > 0.0005) timeStep /= 2;
        break;
      case '4':
        // pause the simulation
        simulate = !simulate;
        break;
    }
  }
};

int main() { MyApp().start(); }
