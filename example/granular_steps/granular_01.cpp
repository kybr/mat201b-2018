// MAT 201B
// Granular examples
// By Karl Yerkes and Matt Wright (2015)
//
// step 1 (Homework 2 Part 2 starter code)

// This code loads a soundfile into a Gamma SamplePlayer object,
// copies the nine 1% portions of the sound into separate SamplePlayer
// objects, and allows the user to trigger either the full sound or
// any single grain with keypresses.
//

#include "allocore/io/al_App.hpp"
#include "Gamma/Gamma.h"
#include "Gamma/SamplePlayer.h"
using namespace al;
using namespace std;

#define NUM_GRAINS (9)

struct AlloApp : App {
  gam::SamplePlayer<> samplePlayer;
  gam::SamplePlayer<> grainPlayer[NUM_GRAINS]; // XXX now an array

  AlloApp() {
    SearchPaths searchPaths;
    searchPaths.addAppPaths();
    searchPaths.addSearchPath(".");

    cout << "Here are all the search paths:" << endl;
    searchPaths.print();

    const char* soundFileName = "matt.wav";
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

    // Make this program's one grain be 1% the duration of the sound file
    //
    int grainSize = samplePlayer.size() / 100;
    cout << "grainSize is " << grainSize << " samples." << endl;

    // Make a Gamma array to hold the samples for each grain
    //
    gam::Array<float> grainArray[NUM_GRAINS];

    // Copy the appropriate samples into our array
    //
    for (int a = 0; a < NUM_GRAINS; ++a) {
      grainArray[a].resize(grainSize, 0);

      for (int sample = 0; sample < grainSize; ++sample) {
        // see diagram
        //
        grainArray[a][sample] = samplePlayer[grainSize * (7 + a) + sample];
      }

      // Set this grain player's buffer to be this array, using the original
      // sample rate
      //
      grainPlayer[a].buffer(grainArray[a], samplePlayer.frameRate(), 1);

      // don't play on startup
      //
      grainPlayer[a].reset();
      grainPlayer[a].phase(0.99999);
    }

    // don't play on startup
    samplePlayer.reset();
    samplePlayer.phase(0.99999);

    initAudio();
    initWindow();
  }

  virtual void onSound(AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      // Play the current sample of the whole file...
      //
      float s = samplePlayer();
      for (int i = 0; i < NUM_GRAINS; ++i) {
        // ...plus the current sample from each grain
        //
        s += grainPlayer[i]();
      }
      io.out(0) = s;
      io.out(1) = s;
    }
  }

  virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    if (k.key() == Keyboard::RETURN) {
      if (samplePlayer.pos() >= samplePlayer.size() - 1) {
        // samplePlayer is currently stopped at the end point, so restart
        //
        samplePlayer.reset();
        cout << "Playing entire sample..." << endl;
      } else {
        // samplePlayer is currently playing, so shut it off
        //
        samplePlayer.phase(0.99999);
        cout << "Stop playing entire sample." << endl;
      }
    } else if (k.key() >= '1' && k.key() <= '9') {
      // Convert from ASCII value of keys '1' through '9' into index from 0 to 8
      //
      int i = k.key() - '1';
      grainPlayer[i].reset();
    }
  }
};

int main() { AlloApp().start(); }
