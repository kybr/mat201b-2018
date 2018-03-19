#include "Gamma/Envelope.h"
#include "Gamma/Filter.h"
#include "Gamma/Gamma.h"
#include "Gamma/Noise.h"
#include "Gamma/Oscillator.h"

#include "allocore/io/al_App.hpp"
#include "allocore/sound/al_Ambisonics.hpp"
#include "allocore/sound/al_Vbap.hpp"
#include "alloutil/al_AlloSphereSpeakerLayout.hpp"

using namespace al;
using namespace std;
using namespace gam;

struct NotchedNoise {
  Accum<> tmr;      // Timer for resetting envelope
  NoisePink<> src;  // Source to envelope
  Decay<> env;      // Exponentially decaying envelope
  Biquad<> bq;      // Biquad filter
  double amplitude, period;
  NotchedNoise(double cutoff, double res, double period) {
    amplitude = 0.707;
    this->period = period;
    bq.type(BAND_PASS);
    bq.freq(cutoff);
    bq.res(res);  // Set resonance of filter
    tmr.period(period);
    tmr.phaseMax();
  }

  float operator()() {
    if (tmr()) {
      env.decay(period);
      env.reset(amplitude);  // Reset envelope and specify amplitude
    }

    // Envelope source
    return bq(src() * env());
  }
};

#define BLOCK_SIZE (4096)
#define SAMPLE_RATE (44100)

static SpeakerLayout* speakerLayout;
static Spatializer* panner;
static Listener* listener;
static SoundSource source;
static AudioScene scene(BLOCK_SIZE);

struct Appp : App {
  NotchedNoise notchedNoise = NotchedNoise(760, 2, 1.0);

  Accum<> tmr;
  SineD<> decayingSine;

  double radius = 20;
  bool movePerSample = true;
  bool bypass = true;
  bool cleanSignal = true;

  Light light;
  Mesh sphere;

  Appp() {
    addSphere(sphere);
    sphere.generateNormals();
    light.pos(7, 7, 7);

    tmr.period(1);
    decayingSine.freq(555);
    decayingSine.decay(1);

    bool inAlloSphere = system("ls /alloshare >> /dev/null 2>&1") == 0;
    // AudioDevice::printAll();
    // audioIO().print();

    if (inAlloSphere) {
      cout << "Using 3 speaker layout" << endl;
      speakerLayout = new AlloSphereSpeakerLayout();
      panner = new Vbap(*speakerLayout);
      // panner = new AmbisonicsSpatializer(*speakerLayout, 3, 3);
      // dynamic_cast<Vbap*>(panner)->setIs3D(false);  // no 3d!
    } else {
      // cout << "Using Headset Speaker Layout" << endl;
      // speakerLayout = new HeadsetSpeakerLayout();
      cout << "Using Stereo Speaker Layout" << endl;
      speakerLayout = new StereoSpeakerLayout();
      panner = new StereoPanner(*speakerLayout);
    }

    panner->print();
    listener = scene.createListener(panner);
    listener->compile();  // XXX need this?

    source.nearClip(1);
    source.farClip(50);
    source.law(ATTEN_NONE);
    // source.law(ATTEN_LINEAR);
    // source.law(ATTEN_INVERSE);
    // source.law(ATTEN_INVERSE_SQUARE);
    source.dopplerType(DOPPLER_NONE);

    scene.addSource(source);
    // scene.usePerSampleProcessing(false);
    scene.usePerSampleProcessing(true);

    if (inAlloSphere) {
      audioIO().device(AudioDevice("ECHO X5"));
      initAudio(SAMPLE_RATE, BLOCK_SIZE, 60, 60);
    } else {
      initAudio(SAMPLE_RATE, BLOCK_SIZE, 2, 2);
    }
    cout << "Audio Device: " << endl;
    audioIO().print();

    initWindow();
  }

  virtual void onSound(AudioIOData& io) {
    double sampleRate = audioIO().fps();
    gam::Sync::master().spu(sampleRate);

    static double t = 0;
    if (!movePerSample) {
      source.pos(radius * sin(t), 0, -radius * cos(t));
      t += float(BLOCK_SIZE) / sampleRate;
    }

    while (io()) {
      if (movePerSample) {
        source.pos(radius * sin(t), 0, -radius * cos(t));
        t += float(1) / sampleRate;
      }

      if (tmr()) decayingSine.reset();

      double s = 0;

      if (cleanSignal)
        s = decayingSine();
      else
        s = notchedNoise();

      if (bypass)
        for (int i = 0; i < io.channelsOut(); i++) io.out(i) = s;
      else
        source.writeSample(s);
    }

    if (!bypass) {
      io.frame(0);
      listener->pos(0, 0, 0);
      scene.render(io);
    }
  }

  virtual void onAnimate(double dt) {
    //
    //
    //
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    //
    //
    //
    light();
    g.translate(source.pos());
    g.draw(sphere);
  }

  virtual void onKeyDown(const ViewpointWindow& w, const Keyboard& k) {
    if (k.key() == 'n') cleanSignal = !cleanSignal;
    if (k.key() == 'b') bypass = !bypass;
    if (k.key() == 'p') movePerSample = !movePerSample;
  }
};

int main() { Appp().start(); }
