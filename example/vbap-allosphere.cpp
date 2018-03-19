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
  NotchedNoise notchedNoise = NotchedNoise(760, 2, 0.4);

  Accum<> decayingSineReset;
  SineD<> decayingSine;

  double radius = 20;
  bool movePerSample = true;
  bool bypass = true;
  bool cleanSignal = true;
  bool perSampleProcessing = true;

  int blockSize = BLOCK_SIZE;
  Light light;
  Mesh sphere;

  Appp() {
    addSphere(sphere);
    sphere.generateNormals();
    light.pos(7, 7, 7);

    decayingSineReset.period(0.4);
    decayingSine.freq(440);
    decayingSine.decay(0.4);

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
    source.dopplerType(DOPPLER_NONE);

    scene.addSource(source);
    scene.usePerSampleProcessing(perSampleProcessing);

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

      if (decayingSineReset()) decayingSine.reset();

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
    cout << "#######################################" << endl;
    if (k.key() == 'n') cleanSignal = !cleanSignal;
    if (k.key() == 'b') bypass = !bypass;
    if (k.key() == 'm') movePerSample = !movePerSample;
    if (k.key() == 'p') {
      perSampleProcessing = !perSampleProcessing;
      scene.usePerSampleProcessing(perSampleProcessing);
    }

    cout << "cleanSignal:" << cleanSignal << endl;
    cout << "bypass:" << bypass << endl;
    cout << "movePerSample:" << movePerSample << endl;
    cout << "perSampleProcessing:" << perSampleProcessing << endl;

    switch (k.key()) {
      default:
      case '0':
        source.law(ATTEN_NONE);
        cout << "source.law(ATTEN_NONE);" << endl;
        break;
      case '1':
        source.law(ATTEN_LINEAR);
        cout << "source.law(ATTEN_LINEAR);" << endl;
        break;
      case '2':
        source.law(ATTEN_INVERSE);
        cout << "source.law(ATTEN_INVERSE);" << endl;
        break;
      case '3':
        source.law(ATTEN_INVERSE_SQUARE);
        cout << "source.law(ATTEN_INVERSE_SQUARE);" << endl;
        break;
    }

    if (k.key() == '-' || k.key() == '_') {
      blockSize /= 2;
      cout << "blockSize:" << blockSize << endl;
      restartAudio();
      cout << "framesPerBuffer:" << audioIO().framesPerBuffer() << endl;
    }
    if (k.key() == '=' || k.key() == '+') {
      blockSize *= 2;
      cout << "blockSize:" << blockSize << endl;
      restartAudio();
      cout << "framesPerBuffer:" << audioIO().framesPerBuffer() << endl;
    }
  }

  void restartAudio() {
    scene.numFrames(blockSize);
    audioIO().close();
    audioIO().framesPerBuffer(blockSize);
    audioIO().start();
  }
};

int main() { Appp().start(); }
