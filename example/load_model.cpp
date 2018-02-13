#include "allocore/graphics/al_Asset.hpp"
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
  Light light;
  Mesh model;

  AlloApp() {
    nav().pos(0, 0, 10);
    light.pos(0, 7, 10);

    Scene* scene = Scene::import(fullPathOrDie("brain-simple-mesh.obj", ".."));
    if (scene == 0) {
      printf("Error reading model\n");
      exit(-1);
    }

    Vec3f min, max, center;
    scene->getBounds(min, max);
    center = (min + max) / 2.f;

    cout << "min =" << min << endl;
    cout << "max =" << max << endl;
    cout << "center  =" << center << endl;

    scene->dump();
    float scale = max[0] - min[0];
    scale = al::max(max[1] - min[1], scale);
    scale = al::max(max[2] - min[2], scale);
    scale = 2.f / scale;

    for (unsigned i = 0; i < scene->meshes(); i++) {
      scene->mesh(i, model);
    }

    cout << "vertices =" << model.vertices().size() << endl;
    cout << "indicies =" << model.indices().size() << endl;
    cout << "normals  =" << model.normals().size() << endl;

    model.scale(scale);

    initWindow();
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    light();
    g.draw(model);
  }
};

int main(int argc, char* argv[]) {
  AlloApp app;
  app.start();
  return 0;
}
