// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use shaders in AlloSystem
// - Render a "point cloud"
//

#include "allocore/io/al_App.hpp"
using namespace al;

// below, 'vertexSource' is a string (aka char*) containing the GLSL source
// code (not C++ source code) of a "vertex shader" (aka vertex program). this
// is a program that runs on the GPU for each vertex you send. say you make a
// Mesh, add 7 vertices and then draw the Mesh. the program below would run 7
// times, once for each vertex in the Mesh. each time, the position of the
// vertex would be in the variable gl_Vertex.
//
// generally, the goal of a vertex program, is to transform the given vertex
// position into "clip space", the rectangle that goes from (-1, -1) to (1, 1),
// so that the GPU can generate "fragments" (see fragmentShader below).
// however, the vertex program can operate on any vertex information (e.g.,
// position, normal, color, texture coordinates, etc.).
//
// the goal of this particular vertex program is to adjust the size of an
// OpenGL point based on the distance between the point (a vertex) and the
// viewer. this gives the viewer a sense of perspective for a point cloud.
//
const char* vertexSource = R"(
  varying vec4 vy_Color;

  // aesthetic scale factor. point "bigness"
  //
  float scaleFactor = 50.0;

  void main() {
    // standard vertex transform. the minimal vertex shader just does this.
    //
    gl_Position = ftransform();

    // determine distance from viewer to point and set point size.
    //
    float distance = length((gl_ModelViewMatrix * gl_Vertex).xyz);
    float pointSize = scaleFactor / distance;

    // don't let points get below 1 pixel.
    //
    gl_PointSize = max(pointSize, 1.0);

    // when points would be smaller than 1 pixel, make them dim
    //
    vy_Color = gl_Color;
    vy_Color.a = min(pointSize, 1.0);
  }
)";

// this is a fragment shader. this program will run on each "fragment". a
// fragment is just a pixel that needs a color. when the GPU draws stuff on the
// screen, it only does as much work as it needs to. if you draw a triangle,
// all the pixels on the edge and inside the triangle are fragments and all the
// other pixels are not.
//
// generally, the goal of a fragment program is to use available information to
// set the color of the fragment. variables passed from vertex program to
// fragment program are *interpolated*.
//
// this is a very minimal fragment program. it just colors the fragment
// acording to the color given by the vertex program.
//
const char* fragmentSource = R"(
  varying vec4 vy_Color;
  void main() {
    gl_FragColor = vy_Color;
  }
)";

#define N (1000)
#define CLOUD_WIDTH (5.0)

struct AlloApp : App {
  ShaderProgram shaderProgram;
  Shader vertexShader, fragmentShader;
  Mesh pointMesh;

  AlloApp() {
    for (int i = 0; i < N; i++) {
      pointMesh.vertex(
          Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) *
          CLOUD_WIDTH);
      pointMesh.color(HSV(rnd::uniform(), 1.0, 1.0));
    }
    initWindow();
  }

  virtual void onAnimate(double dt) {}

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    // GLSL programs are compiled by the GPU while your C++ program is running.
    // in AlloSystem, this is how we compile a vertex program and fragment
    // program (you always have to have both) into a whole program.
    //
    static bool hasRunOnce = false;
    if (!hasRunOnce) {
      vertexShader.source(vertexSource, Shader::VERTEX).compile().printLog();
      shaderProgram.attach(vertexShader);
      fragmentShader.source(fragmentSource, Shader::FRAGMENT)
          .compile()
          .printLog();
      shaderProgram.attach(fragmentShader);
      shaderProgram.link().printLog();
      hasRunOnce = true;
    }

    // to ensure that your GLSL program is run on your Mesh, use .begin() and
    // .end() around your draw call.
    //
    shaderProgram.begin();
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    g.antialiasing(Graphics::NICEST);
    g.depthTesting(true);
    g.blending(true);
    g.blendModeTrans();
    g.draw(pointMesh);
    shaderProgram.end();
  }
};

int main() { AlloApp().start(); }
