// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//
// Shows how to:
// - Parse a file that's a .csv of floats
//
// This code does not...
// - handle bad (i.e., non-float) values
// - verify that all the rows are the same length
//

#include <fstream>
#include <sstream>
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  if (filePath == "") {
    fprintf(stderr, "FAIL\n");
    exit(1);
  }
  return filePath;
}

struct AlloApp : App {
  AlloApp() {
    // make a vector of vectors of floats
    //
    vector<vector<float>> data;

    // find and open the data file and die unless we have it open
    //
    ifstream f(fullPathOrDie("data.csv", ".."), ios::in);
    if (!f.is_open()) {
      cout << "file not open" << endl;
      exit(1);
    }

    // for each line in the file
    //
    string line;
    while (getline(f, line)) {
      // push an empty "row" on our vector of vectors
      //
      data.push_back(vector<float>());

      // for each item in the line
      //
      stringstream stream(line);
      float v;
      while (stream >> v) {
        // push each value onto the current row
        data.back().push_back(v);

        if (stream.peek() == ',') stream.ignore();
      }
    }

    // close the file - we're done
    //
    f.close();

    // print out the data using ' ' as the delimiter
    //
    for (auto line : data) {
      for (auto item : line) {
        cout << item << " ";
      }
      cout << endl;
    }

    // alternatively, we can use (row, column) indexes
    //
    for (int row = 0; row < data.size(); row++) {
      for (int column = 0; column < data[row].size(); column++) {
        cout << data[row][column] << " ";
      }
      cout << endl;
    }
  }
};

int main() { AlloApp().start(); }
