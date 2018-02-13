// MAT201B, final project sketches
// Winter 2016
// Author: Mark Hirsch
// code developed from 'csv.cpp' example by Karl Yerkes

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Gamma/Oscillator.h"
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

struct AlloApp : App {
  Mesh pointMesh;

  Vec3f mapPos[31561];
  vector<vector<float>> data;
  float lat[31561];
  float lon[31561];

  AlloApp() {
    ifstream f(fullPathOrDie("diatoms_fulltime.csv", "../media"), ios::in);
    if (!f.is_open()) {
      cout << "file not open" << endl;
      exit(1);
    }

    string line;
    while (getline(f, line)) {
      data.push_back(vector<float>());

      stringstream stream(line);
      float v;
      while (stream >> v) {
        data.back().push_back(v);

        if (stream.peek() == ',') stream.ignore();
      }
    }
    f.close();

    //*****************XXX

    for (int row = 0; row < data.size(); row++) {
      for (int column = 0; column < data[row].size(); column += 14) {
        // cout << data[row][column] << endl;
        lat[row] = data[row][column];
        // mapPos.x = data[row][column];
      }
    }
    for (int row = 0; row < data.size(); row++) {
      for (int column = 1; column < data[row].size(); column += 14) {
        // cout << data[row][column] << endl;
        lon[row] = data[row][column];
        // mapPos.y = data[row][column];
      }
    }

    // divide lat and lon by their max values to normalize
    for (int i = 0; i < 31561; i++) {
      mapPos[i] = Vec3f(lat[i] / 60, lon[i] / 180, 0.0);
    }

    //  for(int i=0; i<31561; i++){
    //    pointMesh.vertex(lat[i]/60,lon[i]/180,0);
    //    //cout << mapPos[i] << endl;
    //  }

    pointMesh.vertex(0, 0, 0);

    //******************XXX

    initWindow();
  }

  virtual void onAnimate(double dt) {}

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    for (int i = 0; i < 31561; i++) {
      g.pushMatrix();
      g.color(1, 1, 1);
      //  g.translate(lat[i]/60,lon[i]/180,0.0);
      g.translate(mapPos[i]);
      g.scale(0.6);
      g.draw(pointMesh);
      g.popMatrix();
    }
  }
};

int main() { AlloApp().start(); }
