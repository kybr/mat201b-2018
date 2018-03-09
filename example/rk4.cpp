#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

const double sampleRate = 44100;

namespace p5 {
template <typename V>
V map(V v, V low, V high, V low_, V high_) {
  return (v - low) / (high - low) * (high_ - low_) + low_;
}
}  // namespace p5
namespace m7 {
double mtof(double m) { return 8.175799 * pow(2.0, m / 12.0); }
double ftom(double f) { return 12.0 * log2(f / 8.175799); }
double dbtoa(double db) { return 1.0 * pow(10.0, db / 20.0); }
double atodb(double a) { return 20.0 * log10(a / 1.0); }
}  // namespace m7

template <typename Float, typename State>
State euler(const Float t0, const Float dt, const State u0,
            function<State(Float, State)> f) {
  // y_n1 = y_n0 + h * f(t, y)
  return u0 + dt * f(t0, u0);
}

template <typename Float, typename State>
State rk4(const Float t0, const Float dt, const State u0,
          function<State(Float, State)> f) {
  State f0 = f(t0, u0);

  Float t1 = t0 + dt / 2.0;
  State u1 = u0 + dt * f0 / 2.0;
  State f1 = f(t1, u1);

  Float t2 = t0 + dt / 2.0;
  State u2 = u0 + dt * f1 / 2.0;
  State f2 = f(t2, u2);

  Float t3 = t0 + dt;
  State u3 = u0 + dt * f2;
  State f3 = f(t3, u3);

  return u0 + dt * (f0 + f1 * 2.0 + f2 * 2 + f3) / 6.0;
}

struct MassSpring {
  Vec2d state{0};
  double springFactor{0}, dampingFactor{0};
  double f = 0, r = 0;

  enum Method { EULER, RK4, VERLET, DEFAULT } method{DEFAULT};

  void reset(double velocity, double position = 0) {
    state = {position, velocity};
  }
  void recalculate() {
    double _f = f;
    _f /= (sampleRate / 2);
    _f *= 2 * M_PI;
    _f /= 2;  // this factor of 2 is mysterious :(

    // the natural frequency of *damped* harmonic oscillators is
    // not as straightforward as simple harmonic oscillators;
    // you have to do some math...
    double w0 = _f / sqrt(1 - r * r);
    springFactor = w0 * w0;
    dampingFactor = r * 2 * w0;

    cout << "springFactor:" << springFactor << endl;
    cout << "dampingFactor:" << dampingFactor << endl;
    cout << "f:" << f << endl;
    cout << "r:" << r << endl;
    cout << "w0:" << w0 << endl;
  }

  void set(double f_, double r_) {
    f = f_;
    r = r_;
    recalculate();
  }

  void frequency(double f_) {
    f = f_;
    recalculate();
  }

  // make the next sample
  //
  double operator()() {
    switch (method) {
      case RK4: {
        auto dSdt = [=](double t, Vec2d s) -> Vec2d {
          return {s.y, -springFactor * s.x + -dampingFactor * s.y};
        };
        state = rk4<double, Vec2d>(0, 1, state, dSdt);
      } break;

      case EULER: {
        auto dSdt = [=](double t, Vec2d s) -> Vec2d {
          return {s.y, -springFactor * s.x + -dampingFactor * s.y};
        };
        const int N = 500;
        const double dt = 1.0 / N;
        for (int i = 0; i < N; i++)
          state = euler<double, Vec2d>(0, dt, state, dSdt);
      } break;

        // http://www.lonesock.net/article/verlet.html
        // https://www.saylor.org/site/wp-content/uploads/2011/06/MA221-6.1.pdf
        // https://en.wikipedia.org/wiki/Verlet_integration
        // http://codeflow.org/entries/2010/aug/28/integration-by-example-euler-vs-verlet-vs-runge-kutta/
      case VERLET: {
        const int N = 500;
        const double dt = 1.0 / N;
        double lastX = state.x;
        for (int i = 0; i < N; i++) {
          double v = (state.x - lastX);
          state.x = state.x + v +
                    (-springFactor * state.x + -dampingFactor * v) * dt * dt;
          lastX = state.x;
        }

      } break;

      default: {
        state.x += state.y;
        state.y += -springFactor * state.x + -dampingFactor * state.y;
      }
    }
    return state.x;
  }
};

struct AlloApp : App {
  MassSpring thing;

  AlloApp() {
    thing.set(440, 1e-2);
    initAudio();
    initWindow();
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {}

  struct Periodic {
    double time, period;
    bool operator()(float dt) {
      time += dt;
      if (time > period) {
        time -= period;
        return true;
      }
      return false;
    }
  } periodic{0, 0.4};

  virtual void onAnimate(double dt) {
    if (periodic(dt)) {
      thing.reset(0.01);
    }
  }
  virtual void onSound(AudioIOData& io) {
    while (io()) {
      float f = 0;
      f = thing();
      io.out(0) = io.out(1) = f;
    }
  }

  virtual void onMouseMove(const ViewpointWindow& w, const Mouse& m) {
    // normalize mouse position from -1.0 to 1.0
    double r = 1 / m7::mtof(p5::map<double>(m.x(), 0, w.width(), 0, 127));
    double f = m7::mtof(p5::map<double>(m.y(), 0, w.height(), 0, 127));
    thing.set(f, r);
  }
  void onKeyDown(const ViewpointWindow& w, const Keyboard& k) {
    switch (k.key()) {
      case '1':
        thing.method = MassSpring::Method::DEFAULT;
        break;
      case '2':
        thing.method = MassSpring::Method::EULER;
        break;
      case '3':
        thing.method = MassSpring::Method::RK4;
        break;
      case '4':
        thing.method = MassSpring::Method::VERLET;
        break;
    }
  }
};

int main() { AlloApp().start(); }
