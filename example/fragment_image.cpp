// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Load a set of images
// - Break it into fragments (square meshes with correct texture coordinates)
// - Use c++ lambda functions
//

#include <cassert>  // gets you assert()
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

const char* fileList[] = {"chart.png", "surround.jpg", "pluto.png"};

#define N (sizeof(fileList) / sizeof(fileList[0]))

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

// each image fragment is both a Mesh and a Pose. this is called multiple
// inheritance. this sort of inheritance is something that c++ has that not many
// other languages have. in this case, we want each fragment to be able to draw
// itself at an arbitrary location and orientation and we want to keep track of
// that information in the fragment.
//
struct ImageFragment : Mesh, Pose {
  // given width, height and texture coordinates, make an image fragment
  //
  void make(float width, float height, float left, float right, float top,
            float bottom) {
    // cout << "making a fragment..." << endl;
    // cout << "  width:" << width << " height:" << height << endl;
    // cout << "   left:" << left << "  right:" << right << endl;
    // cout << "    top:" << top << " bottom:" << bottom << endl;
    // cout << " ------------------------ " << endl;
    vertex(0, 0);
    texCoord(left, bottom);
    vertex(width, 0);
    texCoord(right, bottom);
    vertex(width, height);
    texCoord(right, top);
    vertex(0, height);
    texCoord(left, top);
    index(0);
    index(1);
    index(2);
    index(0);
    index(2);
    index(3);
    primitive(Graphics::TRIANGLES);

    pos(left, top, 0.0f);
  }
  void onDraw(Graphics& g) {
    g.pushMatrix();
    g.translate(pos());
    g.rotate(quat());
    g.draw(*this);
    g.popMatrix();
  }
};

struct FragmentedImage {
  Texture texture;
  ImageFragment* fragment;
  int size;

  void make(const Image& image) {
    // we need to find a way to break the images into pieces such that almost
    // all the pieces are the same size and those that are not the same as the
    // rest are only a few pixels larger or smaller. this means we can draw all
    // the pieces with the same size and probably no one will notice.
    //
    // this function aims to choose a good value on (low, high] that divides
    // 'value' pretty evenly. It does this by iterating across (low, high],
    // increasing the amount of "slop" each iteration.
    //
    // this is a c++ "lambda", a function that is defined "on the fly"
    //
    auto findGoodDivisor = [](int value, int low, int high) {
      for (int slop = 0; /* !!! */; slop++) {
        for (int candidate = low; candidate < high; candidate++) {
          int remainder = value % candidate;
          if (remainder <= slop) {
            cout << "last fragment will be " << slop << " pixels larger."
                 << endl;
            return candidate;
          }
          if (candidate - remainder <= slop) {
            cout << "last fragment will be " << slop << " pixels smaller."
                 << endl;
            return candidate;
          }
        }
      }
    };

    int fragmentPixelsWidth = findGoodDivisor(image.width(), 40, 60);
    int fragmentPixelsHeight = findGoodDivisor(image.height(), 40, 60);
    cout << "fragments will mostly be " << fragmentPixelsWidth << " by "
         << fragmentPixelsHeight << " pixels" << endl;

    // here we don't cut the image up by pixels. we just choose the right
    // texture coordinates so that each Mesh (each fragment is a Mesh) knows
    // what part of the image (texture) it should draw.
    //
    int COLUMNS = ::ceil((float)image.width() / fragmentPixelsWidth);
    int ROWS = ::ceil((float)image.height() / fragmentPixelsHeight);
    fragment = new ImageFragment[ROWS * COLUMNS];
    size = 0;
    for (int row = 0; row < ROWS; row++)
      for (int column = 0; column < COLUMNS; column++) {
        fragment[size].make(1.0f / COLUMNS, 1.0f / ROWS,
                            (float)column / COLUMNS, (column + 1.0f) / COLUMNS,
                            (row + 1.0f) / ROWS, (float)row / ROWS);
        size++;
      }

    texture.allocate(image.array());
  }

  void setZ(float z) {
    for (int i = 0; i < size; i++) fragment[i].pos().z = z;
  }

  void onDraw(Graphics& g) {
    texture.bind();
    for (int i = 0; i < size; i++) fragment[i].onDraw(g);
    texture.unbind();
  }
};

struct AlloApp : App {
  FragmentedImage fragmentedImage[N];

  AlloApp() {
    for (int imageNumber = 0; imageNumber < N; imageNumber++) {
      Image image;
      if (!image.load(fullPathOrDie(fileList[imageNumber], ".."))) {
        cerr << "failed to load " << fileList[imageNumber] << endl;
        exit(1);
      }

      fragmentedImage[imageNumber].make(image);

      // set this fragmented image back one OpenGL unit
      //
      fragmentedImage[imageNumber].setZ(-imageNumber);
    }

    // set the position and orientation of the viewer so they can see all the
    // images clearly. i found these numbers by printing the position of the
    // nav while i used the keyboard to get a good position.
    //
    nav().pos(Vec3d(4.2, 0.5, 1.9));
    nav().quat(Quatd(0.96, 0.00, 0.29, 0.00));

    initWindow();
  }

  virtual void onAnimate(double dt) {
    // 'r' is another c++ lambda function. in this case, it makes allows us to
    // say "r()" which is shorter than saying "rnd::uniformS()" so the code
    // below looks cleaner.
    //
    auto r = []() { return rnd::uniformS(); };
    for (int i = 0; i < N; i++)
      for (int f = 0; f < fragmentedImage[i].size; f++) {
        // make each fragment do a little dance
        //
        fragmentedImage[i].fragment[f].faceToward(Vec3f(r(), r(), r()), 0.01);
        fragmentedImage[i].fragment[f].pos() += Vec3f(r(), r(), r()) * 0.005;
      }
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    for (int i = 0; i < N; i++) fragmentedImage[i].onDraw(g);
  }
};

int main() { AlloApp().start(); }
