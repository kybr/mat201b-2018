#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

Mesh coneMesh, backMesh;
Texture backTexture;

struct Cone : Pose {
  void onDraw(Graphics& g) {
    g.pushMatrix();
    g.translate(pos());
    g.rotate(quat());
    g.draw(coneMesh);
    g.popMatrix();
  }
};

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
  Light light;
  Material material;
  Cone cone;
  vector<Cone> other;

  AlloApp() {
    Image image;
    if (!image.load(fullPathOrDie("worthersee_klagenfurt.jpg"))) {
      fprintf(stderr, "FAIL\n");
      exit(1);
    }
    backTexture.allocate(image.array());

    addSphereWithTexcoords(backMesh, 999);
    backMesh.generateNormals();

    lens().near(0.1);
    lens().far(1000);

    addCone(coneMesh);
    coneMesh.generateNormals();
    initWindow();
    light.pos(5, 5, 5);
    nav().pos(0, 0, 10);

    other.resize(100);
    for (auto& e : other) {
      e.pos(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
      e.pos() *= rnd::uniform(10.0, 60.0);
      e.quat() = Quatd(rnd::uniformS(), rnd::uniformS(), rnd::uniformS(),
                       rnd::uniformS());
      e.quat().normalize();
    }
  }

  void onKeyDown(const ViewpointWindow& vw, const Keyboard& k) {}

  void onAnimate(double dt) {
    //
    nav().faceToward(cone);
    cone.quat() = nav();
    Vec3f v = (cone.pos() - nav());
    float d = v.mag();
    cone.pos() += (v / d) * (10 - d);
  }
  void onDraw(Graphics& g) {
    g.lighting(false);
    g.depthMask(false);
    g.pushMatrix();
    g.translate(nav().pos());
    backTexture.bind();
    //    g.color(1, 1, 1);
    g.draw(backMesh);
    backTexture.unbind();
    g.popMatrix();

    //

    g.depthMask(true);
    material();
    light();  // turns lighting back on

    g.scale(0.2);
    for (auto& c : other) c.onDraw(g);
    g.scale(1 / 0.2);
    cone.onDraw(g);
  }
};

int main() { AlloApp().start(); }
