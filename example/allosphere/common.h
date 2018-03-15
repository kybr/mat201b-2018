#ifndef __COMMON__
#define __COMMON__

#include "allocore/al_Allocore.hpp"
using namespace al;
using namespace std;

struct State {
  Vec3f agentPosition;
  Pose navPose;
};

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  if (filePath == "") {
    std::cerr << "Failed to find " << fileName << std::endl;
    exit(1);
  };
  return filePath;
}

#endif
