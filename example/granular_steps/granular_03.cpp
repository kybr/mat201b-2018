// MAT 201B
// Granular examples
// By Matt Wright and Karl Yerkes (2015)
//
// step 3 (Homework 2 Part 2 starter code)

// This code loads a soundfile into a Gamma SamplePlayer object,
// divides it into 100 grains, computes the RMS of every grain,
// sorts the grains into increasing order of RMS, and copies
// the grains in this new order into another SamplePlayer.
//

// Pull in the functionality this example uses
//
#include "allocore/io/al_App.hpp"
#include "Gamma/Gamma.h"
#include "Gamma/SamplePlayer.h"

// "using namespace" allows us to access names inside these two namespaces
// without having to qualify them like "std::cout"
//
using namespace al;
using namespace std;

// Using #define creates a preprocessor macro, a compile-time "constant" that
// will be implemented through a macro substitution process (search and
// replace) before compilation.
//
#define NUM_GRAINS (20)  // How many grains we want

// A separate structure (which you can think of as a class) to represent a
// Grain taken out of our original sound file. We will make a vector of these
// to sort by RMS.
//
struct Grain {
  // Which input samples are in this grain?
  //
  int start;
  int stop;

  // What is the RMS of those samples?
  //
  float RMS;
};

// The comparison function we'll use to sort grains by RMS: It returns bool
// (true or false) and its arguments are references (&) to Grain objects that
// this function promises not to modify (const). Arbitrarily I name them "one"
// and "other".
//
bool compareGrainByRMS(const Grain& a, const Grain& b) {
  // The less-than (<) operator produces the true or false that this function
  // returns. Specifically we're comparing the RMS of the two grains.
  //
  return a.RMS < b.RMS;
}

// A new subclass of gam::SamplePlayer<> that implements only a single
// additional feature.
//
struct SafeSamplePlayer : gam::SamplePlayer<> {
  // "operator[]" is the method for the array indexing operator, like when you
  // say samplePlayer[i]. The argument is an integer ("unsigned integer of 32
  // bits type") and the return value is float
  //
  float operator[](uint32_t i) {
    // Normally i must be less than size()
    //
    if (i >= size()) {
      // if i is too big, explicitly return zero
      //
      return 0.;
    } else {
      // otherwise look up sample number i in the usual gam::SamplePlayer<> way
      //
      return gam::SamplePlayer<>::operator[](i);
    }
  }
};

static void addWaveformDisplay(Mesh& m, gam::SamplePlayer<>& s) {
  // This variable determines how many audio samples will be represented
  // by each vertex of the mesh:
  //
  int zoomOut = 256;

  // Set the mesh's OpenGL primitive to LINE_STRIP so that the vertices will be
  // rendered as lines (not triangles) connecting each to the next;
  //
  m.primitive(Graphics::LINE_STRIP);

  // index variable to keep track of our position in the sample array
  //
  int n = 0;  // current sample #

  for (int i = 0; i < s.size() / zoomOut; ++i) {
    // Within this group we haven't seen any samples yet, so
    // initialize these max and min variables (representing the
    // largest and smallest sample values we've seen so far inside
    // this group) to dummy values so we will be sure the first sample
    // we see will have a higher "max" and a lower "min"
    //
    float max = -1, min = 1;

    // Now loop through the 256 samples of this group, using index variable "j"
    //
    for (int j = 0; j < zoomOut; ++j) {
      // Do the actual comparisons to see if s[n] (this particular sample) is
      // the new max and/or the new min.
      //
      if (s[n] > max) max = s[n];
      if (s[n] < min) min = s[n];
      n++;
    }

    // A constant to determine how tall (on the screen) the waveform display
    // will appear
    //
    float verticalScale = 0.5;

    // Write two vertices into the mesh to represent the entire 256-sample
    // block. Both have the same x value: (float)i / (s.size() / zoomOut) That
    // represents the current value of i as a fraction of the maximum possible
    // value of i, in other words, i normalized to lie between 0 and 1.
    //
    // The first vertex's y value is the maximum amplitude of this block
    m.vertex((float)i / (s.size() / zoomOut), max * verticalScale, 0);
    // The second vertex's y value is the minimum amplitude of this block
    m.vertex((float)i / (s.size() / zoomOut), min * verticalScale, 0);

    // "cursor"
    //
    int w = 2;
    int p = s.pos() / zoomOut;
    if (p >= i - w && p <= i + w) {
      m.color(RGB(1, 0, 0));
      m.color(RGB(1, 0, 0));
    } else {
      m.color(RGB(1, 1, 1));
      m.color(RGB(1, 1, 1));
    }
  }
}

// This is the class for this particular program's subclass of App aka "our App
// class"
//
struct AlloApp : App {
  // Each AlloApp has three objects inside it:
  //
  // 1) a SafeSamplePlayer object named "samplePlayer" that will play our
  // entire sound file.
  // 2) another SafeSamplePlayer object, named "sortedGrainsPlayer", that will
  // play a granular rearrangement of the audio from our input file.
  // 3) a C++ vector of Grain objects (a type we defined above), which we will
  // use to sort the grains into increasing order of RMS.
  //
  SafeSamplePlayer samplePlayer;
  SafeSamplePlayer sortedGrainsPlayer;
  std::vector<Grain> grainArray;

  // A function to print out all the grains, for debugging. Don't worry about
  // it.  It shows how to use a C++ "iterator" in a for loop to go through an
  // entire std::vector.
  //
  void printGrains() {
    int i = 0;
    for (std::vector<Grain>::iterator it = grainArray.begin();
         it != grainArray.end(); ++it) {
      std::cout << "  grainArray[" << i << "] = [start:" << it->start
                << ", stop:" << it->stop << ", RMS:" << it->RMS << "]" << endl;
      ++i;
    }
  }

  // Since our class is named AlloApp, this method with the same name is the
  // "constructor". It will be called when we create a new AlloApp, so
  // everything here happens once, before the program starts running.
  //
  AlloApp() {
    // Make an object of type SearchPaths and add to it all the "standard"
    // paths that an App should have
    //
    SearchPaths searchPaths;
    searchPaths.addAppPaths();
    searchPaths.addSearchPath(".");

    // Print out those paths
    //
    cout << "Here are all the search paths:" << endl;
    searchPaths.print();

    // Make a variable "soundFileName" that is a C string (char *) that
    // we promise never to modify (const) and whose value is the name of
    // the sound file ("matt.wav").
    const char* soundFileName = "matt.wav";

    // Try to find our sound file (given just its name) in the search path, and
    // save the full file path (including the part that says which directory
    // the file lives in) into a C++ "string" object named soundFilePath.
    //
    string soundFilePath = searchPaths.find(soundFileName).filepath();

    // If the file was not found then soundFilePath will be an empty string
    //
    if (soundFilePath == "") {
      // Complain
      cerr << "FAIL: your sound file " << soundFileName
           << " was not found in the file path." << endl;
      // And give up
      exit(1);
    }

    // Otherwise print soundFilePath so the user can see where the file was
    // found
    cout << "Full path to your sound file is " << soundFilePath << endl;

    // Convert from the C++ string class to an old fashioned C string, and pass
    // the latter to samplePlayer.load so that it will load our sound file.
    // The return value will be true for success or false for failure.
    //
    if (!samplePlayer.load(soundFilePath.c_str())) {
      // Since samplePlayer.load() returned false we didn't load the file, so
      // complain and give up.
      //
      cerr << "FAIL: your sound file did not load." << endl;
      exit(1);
    }

    // Decide how big to make each grain by dividing the total number of
    // samples from our sound file by the desired number of grains:
    //
    int grainSize = samplePlayer.size() / NUM_GRAINS;

    // Since this is integer division it rounds down, so if the duration of the
    // file happens not to be an exact integer multiple of the grain size we'll
    // miss the last few samples. Detect this situation...
    //
    if ((grainSize * NUM_GRAINS) < samplePlayer.size()) {
      // ...and correct it.
      // There are some leftover samples, so make each grain one bigger
      ++grainSize;
    }

    // Print the results of the above computations for the user to see
    cout << "File has " << samplePlayer.size() << " samples, so with "
         << NUM_GRAINS << " grains we have " << grainSize
         << " samples per grain." << endl;

    cout << "  " << NUM_GRAINS << " * " << grainSize << " = "
         << NUM_GRAINS * grainSize << " so the last grain has "
         << (NUM_GRAINS * grainSize) - samplePlayer.size()
         << " implicit zero-valued samples at the end." << endl;

    // Make our vector of Grain objects large enough to hold all our grains
    //
    grainArray.resize(NUM_GRAINS);

    // Loop over all grains computing the RMS of each and saving it
    //
    for (int g = 0; g < NUM_GRAINS; ++g) {
      float sumOfSquares = 0;
      for (int i = 0; i < grainSize; ++i) {
        // Get sample number i from grain number g again we rely on it being OK
        // to read past the end of the buffer
        //
        float s = samplePlayer[g * grainSize + i];
        // Add the square of s to our running total
        //
        sumOfSquares += s * s;
      }

      // Now we've finished looping over all the sample in this block, so the
      // following code happens once per block, not once per sample.
      // sumOfSquares / grainSize is the mean squared sample value; the sqrt of
      // that is the RMS.
      //
      float RMS = sqrt(sumOfSquares / grainSize);

      // Record everything about this particular grain:
      // 1) Starting position (in samples) is g*grainsize;
      // 2) Ending position (in samples) is the starting position plus
      // grainSize-1
      // 3) The RMS of this grain is what we just computed
      grainArray[g].start = g * grainSize;
      grainArray[g].stop = g * grainSize + (grainSize - 1);
      grainArray[g].RMS = RMS;
    }

    // Now we've finished looping over all blocks; so the following code
    // happens only once, not once per block:

    // Print out the grains in their original order (sequential as they appear
    // in the file)
    //
    cout << "Before sort:" << endl;
    printGrains();

    // Now sort the grains in increasing order of RMS
    //
    sort(grainArray.begin(), grainArray.end(), compareGrainByRMS);

    // Print them out a second time, now in increasing order of RMS
    //
    cout << "After sort:" << endl;
    printGrains();

    // Make an "output" array to hold the reassembled grains. (AKA the result
    // of the granular synthesis)
    //
    gam::Array<float> outArray;

    // Note that it will generally be slightly larger than the input file. It
    // needs to be long enough for NUM_GRAINS many grains each of size
    // grainSize
    //
    outArray.resize(NUM_GRAINS * grainSize, 0);

    // Write each grain into the correct spot in the output buffer.

    // Loop over all grains using index variable "grain". Remember that
    // grainArray is now sorted in order of increasing RMS, so as grain
    // increases so will grainArray[grain].RMS
    //
    for (int grain = 0; grain < NUM_GRAINS; ++grain) {
      // Get a (reference to) the next grain in the low-to-high-RMS order
      //
      Grain& g = grainArray[grain];

      // Determine this grain's position in the output sound according to
      // "grain", which is the index of the current grain in the list that has
      // been sorted by RMS. This is the position in outArray of where the
      // zeroth sample of this grain should go.
      //
      int whereToPutThisGrain = grain * grainSize;

      // Put the entire grain in the right place by looping over all the
      // samples in the grain with the index variable "i"
      //
      for (int i = 0; i < grainSize; ++i) {
        // Write a new value into position whereToPutThisGrain+i of the
        // outArray. We can understand whereToPutThisGrain+i as the starting
        // point for this grain plus the index of the current sample of this
        // grain. The sample to write comes from our original sample file,
        // specifically from the current grain of our original sample file,
        // whose starting position we can read out of the Grain object "g".
        // All in all, as i sweeps from 0 to blockSize-1, we will copy all the
        // samples from the right place to the right place.
        //
        outArray[whereToPutThisGrain + i] = samplePlayer[g.start + i];
      }
    }

    // Tell sortedGrainsPlayer to use outArray as its playback buffer
    //
    sortedGrainsPlayer.buffer(outArray, samplePlayer.frameRate(), 1);

    // Set both SamplePlayer objects not to play on startup. Phase goes from 0
    // to (not quite) 1, so set it to almost 1 and the player will begin its
    // life "stuck at the end of its sample"
    //
    samplePlayer.phase(0.99999);
    sortedGrainsPlayer.phase(0.99999);

    // The usual calls to get an al::App to have Audio and a Window
    //
    initAudio();
    initWindow();

    // Set the navigation (camera) position to x=0.5, y=0, z=4
    //
    nav().pos(0.5, 0, 4);
  }

  // This will be called every time the app needs to render pixels to the screen
  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    // Create a temporary mesh named "m"
    Mesh m;

    // Call addWaveformDisplay() to add a waveform of our original sample to m
    addWaveformDisplay(m, samplePlayer);
    // Move up 0.6 units
    g.translate(0, 0.6, 0);
    // Draw our mesh
    g.draw(m);

    // Now throw away all the vertices in the mesh...
    m.reset();
    // ...and write the waveform of our modified sound into m
    addWaveformDisplay(m, sortedGrainsPlayer);

    // Move down 1 unit (so the two waveforms don't overlap)
    g.translate(0, -1, 0);

    // draw the second waveform
    g.draw(m);
  }

  // This will be called every time the app needs to produce new audio samples
  //
  virtual void onSound(AudioIOData& io) {
    // This is the magic line that sets all of Gamma to have the same audio
    // sampling rate as your audio hardware
    //
    gam::Sync::master().spu(audioIO().fps());

    // Loop for as long as we still have to compute more audio samples during
    // the current invocation of onSound...
    while (io()) {
      // Get and add together the current sample of both SamplePlayer objects
      // (allowing us to play the original file at the same time as the
      // granular)
      //
      float s = sortedGrainsPlayer() + samplePlayer();

      // Write the current sample to both the left and right audio output
      // channels
      io.out(0) = io.out(1) = s;
    }
  }

  // This is a helper function that toggles whether a given SamplePlayer is
  // playing.
  void TogglePlayback(gam::SamplePlayer<>& p) {
    // If the current position is at or beyond the last sample in the buffer,
    if (p.pos() >= p.size() - 1) {
      // p is currently stopped at the end point, so restart (at the beginning)
      p.reset();
    } else {
      // p is currently playing, so stop it
      p.phase(0.99999);
    }
  }

  // AlloSystem calls this callback whenever the user types a key with out
  // app's window having focus.
  //
  virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    if (k.key() == Keyboard::RETURN) {  // If that key is the return key...
      // then toggle playback of the one that's playing the original sample
      //
      TogglePlayback(samplePlayer);
    } else if (k.key() == ' ') {  // If that key is the space key...
      // then toggle playback of the one that's playing the granulated version
      //
      TogglePlayback(sortedGrainsPlayer);
    }
  }
};

// The C "main" function is the original entry point to our code.   All it does
// is make an AlloApp and ask it to start.
//
int main() { AlloApp().start(); }
