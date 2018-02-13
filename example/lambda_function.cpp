// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use 'auto'
// - Define a lambda function
// - Use comparison functions to change the behaviour of a sort
//
#include <algorithm>  // sort
#include <iostream>
#include <vector>
using namespace std;

int main() {
  // in c++, functions can be defined on the fly.
  // this makes it possible to define a function
  // near to where it is used. we use the 'auto'
  // keyword because the type of functions looks
  // awful. auto infers types.
  auto say = [](string message) { printf("%s\n", message.c_str()); };
  say("this is a message");
  say("here's another message");
  say("this is my final message");

  // lambdas are particularly useful for making
  // comparison functions for use in sorting...
  //
  vector<int> v{4, 7, 0, 1, 7, 3, 2, 1, 7, 8, 9, 5};
  auto ascending = [](int a, int b) { return a < b; };
  sort(v.begin(), v.end(), ascending);
  for (int i : v) printf("%d ", i);
  printf("\n");

  // now sort in descending order..
  //
  auto descending = [](int a, int b) { return a > b; };
  sort(v.begin(), v.end(), descending);
  for (int i : v) printf("%d ", i);
  printf("\n");
}
