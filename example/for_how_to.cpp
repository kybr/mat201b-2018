// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use a for loop to iterate across a simple array
//   a couple different ways...
// - Illustrates how C++ does _not_ initialize memory for you
//
#include <iostream>

struct Grain {
  int start, stop;
  float data;
};

int main() {
  Grain grain[100];  // std::vector<int> or SamplePlayer

  // traditional, "good old" for loop
  // increment an int and index into an array
  //
  for (int n = 0; n < 100; ++n) {
    printf("%d: %d\n", n, grain[n].start);

    // XXX but what if i have to say "grain[n]" a LOT?
  }

  // fancy, "new hip" for loop
  // still increment and int if we want to
  // we have a reference to the current item
  //
  int n = 0;
  for (Grain& g : grain) {
    printf("%d: %d\n", n++, g.start);

    // XXX "g" is easier to say and see than "grain[n]"
  }
}
