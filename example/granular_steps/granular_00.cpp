// MAT 201B
// Granular examples
// By Karl Yerkes and Matt Wright (2015)
//
// step 0 (Homework 2 Part 2 starter code)

// This code loads a soundfile into a Gamma SamplePlayer object,
// copies the 8th 1% of the sound into a second SamplePlayer object,
// and allows the user to trigger either the full sound or the single grain
// with keypresses.
//

#include "allocore/io/al_App.hpp"
#include "Gamma/Gamma.h"
#include "Gamma/SamplePlayer.h"
using namespace al;
using namespace std;

struct AlloApp : App {
  gam::SamplePlayer<> samplePlayer;
  gam::SamplePlayer<> grainPlayer;

  AlloApp() {
    SearchPaths searchPaths;
    searchPaths.addAppPaths();
    searchPaths.addSearchPath(".");

    cout << "Here are all the search paths:" << endl;
    searchPaths.print();

    // Choose a sound...
    const char *soundFileName = "matt.wav";
    // const char *soundFileName = "SoundDemo.wav";
    //const char* soundFileName = "kauaiOo.wav";

    string soundFilePath = searchPaths.find(soundFileName).filepath();
    if (soundFilePath == "") {
      cerr << "FAIL: your sound file " << soundFileName
           << " was not found in the file path." << endl;
      exit(1);
    }

    cout << "Full path to your sound file is " << soundFilePath << endl;
    if (!samplePlayer.load(soundFilePath.c_str())) {
      cerr << "FAIL: your sound file did not load." << endl;
      exit(1);
    }

    cout << "Loaded sound file; it has " << samplePlayer.size() << " samples."
         << endl;

    if (samplePlayer.channels() != 1) {
      cerr << "FAIL: your sound file has " << samplePlayer.channels()
           << " channels but I can only handle mono; sorry!" << endl;
      exit(1);
    }

    // Make this program's one grain be 1% the duration of the sound file
    //
    int grainSize = samplePlayer.size() / 100;
    cout << "grainSize is " << grainSize << " samples." << endl;

    // Make a Gamma array to hold the samples for our grain
    //
    gam::Array<float> array;
    array.resize(grainSize, 0);

    // Copy the appropriate samples into our array
    //
    for (int i = 0; i < grainSize; ++i) {
      array[i] = samplePlayer[grainSize * 7 + i];
    }

    // Set the grain player's buffer to be our array, using the original
    // samplerate
    //
    grainPlayer.buffer(
        array, samplePlayer.frameRate(),
        1 /* mono files only please, unless you want to get fancy */);

    // don't play on startup
    //
    samplePlayer.reset();
    samplePlayer.phase(1.0);
    grainPlayer.phase(1.0);

    initAudio();
    initWindow();
  }

  virtual void onSound(AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      // Play the current sample of both the grain and the whole sample
      //
      float s = (grainPlayer() + samplePlayer()) / 2;
      io.out(0) = s;
      io.out(1) = s;
    }
  }

  virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    if (k.key() == Keyboard::RETURN) {
      if (samplePlayer.pos() >= (samplePlayer.frames() - 1)) {
        // samplePlayer is currently stopped at the end point, so restart
        //
        samplePlayer.reset();
        cout << "Playing entire sample..." << endl;
      } else {
        // samplePlayer is currently playing, so shut it off
        //
        samplePlayer.phase(1.0);
        cout << "Stop playing entire sample." << endl;
      }
    } else {
      // Choose a random playback rate between 0.5 (half speed = one octave
      // lower) and 2.0 (double speed = one octave higher)
      //
      grainPlayer.rate(rnd::uniform(0.5, 2.0));
      grainPlayer.reset();
    }
  }
};

int main() { AlloApp().start(); }
