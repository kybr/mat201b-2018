#include <future>
#include <iostream>
using namespace std;

int called_from_async() {
  cout << "Async call" << endl;
  return 42;
}

int main() {
  // called_from_async launched in a separate thread if possible
  future<int> result(async(called_from_async));

  cout << "Message from main." << endl;

  // ensure that called_from_async is launched synchronously
  // if it wasn't already launched
  //  cout << "result was " << result.get() << endl;

  return 0;
}
