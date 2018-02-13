// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Write a very simple "Unit Generator"
//   + https://en.wikipedia.org/wiki/Unit_generator
// - Write a "functor"
//   + https://en.wikipedia.org/wiki/Function_object#In_C_and_C.2B.2B
//

#include <iostream>
using namespace std;

struct Pulse {
  unsigned period;
  unsigned sample;

  Pulse(unsigned p = 10) {
    period = p;
    sample = 0;
  }

  float generateNextSample() {
    sample++;
    if (sample % period == 0)
      return 1.0f;
    else
      return 0.0f;
  }

  // allow this class to be used like a function
  //
  float operator()() { return generateNextSample(); }
};

int main() {
  Pulse pulse;
  for (int i = 0; i < 1000; i++)
    cout << pulse() << endl;  // looks like a function call
}
