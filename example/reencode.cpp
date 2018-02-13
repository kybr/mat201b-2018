// vim: :ClangFormatAutoDisable
//
#include "Gamma/Gamma.h"
#include "Gamma/SoundFile.h"
//#include "Gamma/SamplePlayer.h"
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;
using namespace gam;

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

string fullPath(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  return filePath;
}

const char* bFormatAmbisonicFileName = "SilkBoardWXYZ.wav";

void makeSingleFileFromMany() {
  SoundFile w(fullPathOrDie("SilkBoardOmni.wav", ".."));
  SoundFile x(fullPathOrDie("SilkBoardX.wav", ".."));
  SoundFile y(fullPathOrDie("SilkBoardY.wav", ".."));
  SoundFile z(fullPathOrDie("SilkBoardZ.wav", ".."));
  w.openRead();
  x.openRead();
  y.openRead();
  z.openRead();
  w.print();

  if (1 != w.channels() || 1 != x.channels() || 1 != y.channels() ||
      1 != z.channels()) {
    cerr << "Fail: bad channel count." << endl;
    exit(1);
  }

  int F = w.frames();
  if (F != x.frames() || F != y.frames() || F != z.frames()) {
    cerr << "Fail: frame counts do no match." << endl;
    exit(1);
  }

  int* data = new int[F * 4];
  int* buffer = new int[F];
  w.readAll(buffer);
  for (int i = 0; i < F; i++) data[i * 4 + 0] = buffer[i];
  x.readAll(buffer);
  for (int i = 0; i < F; i++) data[i * 4 + 1] = buffer[i];
  y.readAll(buffer);
  for (int i = 0; i < F; i++) data[i * 4 + 2] = buffer[i];
  z.readAll(buffer);
  for (int i = 0; i < F; i++) data[i * 4 + 3] = buffer[i];

  SoundFile out(bFormatAmbisonicFileName);
  out.encoding(w.encoding())
      .format(w.format())
      .frameRate(w.frameRate())
      .channels(4)
      .openWrite();
  out.write(data, F);
  out.print();
}

struct AlloApp : App {
  AlloApp() {
    string inputFile = fullPath(bFormatAmbisonicFileName, "..");

    if (!inputFile) {
      makeSingleFileFromMany();
      inputFile = fullPathOrDie(bFormatAmbisonicFileName, "..");
    }

    SamplePlayer i;
    if (!i.load(inputFile)) {
      cerr << "Failed to load sound file" << endl;
      exit(1);
    }

    SamplePlayer o;

    for (int k = 0; k < i.frames(); k++) {
      //       W      X        Y        Z
      o( 0) = i(0) + i(1)            + i(3); /////// TOP RING //////
      o( 1) = i(0) + i(1)   - i(2)   + i(3);
      o( 2) = i(0)          - i(2)   + i(3);
      o( 3) = i(0) - i(1)   - i(2)   + i(3);
      o( 4) = i(0) - i(1)            + i(3);
      o( 5) = i(0) - i(1)   + i(2)   + i(3);
      o( 6) = i(0)          + i(2)   + i(3);
      o( 7) = i(0) + i(1)   + i(2)   + i(3);
      o( 8) = i(0) + i(1)            - i(3); /////// BOTTOM RING //////
      o( 9) = i(0) + i(1)   - i(2)   - i(3);
      o(10) = i(0)          - i(2)   - i(3);
      o(11) = i(0) - i(1)   - i(2)   - i(3);
      o(12) = i(0) - i(1)            - i(3);
      o(13) = i(0) - i(1)   + i(2)   - i(3);
      o(14) = i(0)          + i(2)   - i(3);
      o(15) = i(0) + i(1)   + i(2)   - i(3);
      o(16) = i(0) + i(1)                  ; /////// Center Ring //////
      o(17) = i(0) + i(1)   - i(2)/2       ;
      o(18) = i(0) + i(1)   - i(2)         ;
      o(19) = i(0) + i(1)/2 - i(2)         ;
      o(20) = i(0)          - i(2)         ;
      o(21) = i(0) - i(1)/2 - i(2)         ;
      o(22) = i(0) - i(1)   - i(2)         ;
      o(23) = i(0) - i(1)   - i(2)/2       ;
      o(24) = i(0) - i(1)                  ;
      o(25) = i(0) - i(1)   + i(2)/2       ;
      o(26) = i(0) - i(1)   + i(2)         ;
      o(27) = i(0) - i(1)/2 + i(2)         ;
      o(28) = i(0)          + i(2)         ;
      o(29) = i(0) + i(1)/2 + i(2)         ;
      o(30) = i(0) + i(1)   + i(2)         ;
      o(31) = i(0) + i(1)   + i(2)/2       ;
    }
  }
};

int main() {
  // AlloApp().start();

  separateFilesToOne();
}
