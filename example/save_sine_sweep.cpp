// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use Gamma's SoundFile class to save a sound file
//

#include "Gamma/Oscillator.h"
#include "Gamma/SoundFile.h"
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;
using namespace gam;

struct MyApp : App {
  // This constructor is the only thing this program does (see main).
  //
  MyApp() {
    // Provide some parameters like sound file length and sample rate.
    //
    float lengthSeconds = 0.9f;
    int sampleRate = 44100;

    // Calculate the number of frames that is and allocate an array of floats
    // to hold the data.
    //
    int frameCount = int(lengthSeconds * sampleRate) + 1;
    float* data = new float[frameCount];

    // First we need to make some sound data. In your code, you'll do this a
    // different way. Here, I want to make a sine wave sweep from the Nyquist
    // frequency (sampleRate / 2) down to 20 Hz. To do this, we...
    //

    // Make a Gamma sine object and tell it the sample rate
    //
    Sine<> sine;
    Sync::master().spu(sampleRate);

    // Set the initial frequency
    //
    float frequency = sampleRate / 2.0f;
    sine.freq(frequency);

    // Fill the float array with a sine sweep
    //
    for (int i = 0; i < frameCount; i++) {
      data[i] = sine();
      sine.freq(frequency);

      // Use linear interpolation to sweep
      //
      float t = (float)i / frameCount;
      frequency = 20.0f * t + (sampleRate / 2.0f) * (1 - t);
    }

    // We have our data. Now it's time to save this data to a sound file.
    // First, choose an output file name. Then make a sound file object and set
    // various parameters: format, channels, sample rate, etc.
    //
    const char* fileName = "outputFile.wav";
    SoundFile soundFile(fileName);
    soundFile.format(SoundFile::WAV);
    soundFile.encoding(SoundFile::PCM_16);
    soundFile.channels(1);
    soundFile.frameRate(sampleRate);

    // Open the sound file for writing. This could fail. If so, we exit.
    //
    if (!soundFile.openWrite()) {
      cerr << "Could not open " << fileName << " for writing." << endl;
      exit(-1);
    }

    // Write the sound file. This could fail. If so, we exit.
    //
    if (soundFile.write(data, frameCount) != frameCount) {
      cerr << "Failed to write " << fileName << endl;
      exit(-1);
    }

    cout << "Look for a file named '" << fileName << "'" << endl;

    // Clean up. Close the sound file and deallocate the float array.
    //
    soundFile.close();
    delete[] data;
  }
};

// Because we do not call start---We only make a MyApp---, our program
// completes after the constructor is called.
//
int main() { MyApp(); }
