#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
using namespace al;

#define N (1000)
#define CLOUD_WIDTH (5.0)

struct MyApp : OmniStereoGraphicsRenderer {
  Mesh pointMesh;
  double t = 0;

  MyApp() {
    auto r = []() { return rnd::uniformS(); };
    pointMesh.primitive(Graphics::POINTS);
    for (int i = 0; i < N; i++) {
      pointMesh.vertex(Vec3f(r(), r(), r()) * CLOUD_WIDTH);
      pointMesh.color(HSV(rnd::uniform(), 1.0, 1.0));
    }

    omni().clearColor() = Color(0.15);

    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) {
    pose = nav();
    t += dt;
  }

  virtual void onDraw(Graphics& g) {
    shader().uniform("pointScale", 50.0f);
    shader().uniform("time", (float)t);

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    g.antialiasing(Graphics::NICEST);
    // g.depthTesting(true);
    // g.blending(true);
    // g.blendModeTrans();
    g.draw(pointMesh);
  }

  std::string vertexCode() {
    return R"(

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    //vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    //vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
 vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
    vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}


      varying vec4 vy_Color;
      uniform float pointScale;
      uniform float time;

      void main(){
        vec4 vertex = gl_ModelViewMatrix * gl_Vertex;
        gl_Position = omni_render(vertex);


        // determine distance from viewer to point and set point size.
        //
        float pointSize = pointScale / length(vertex.xyz);

        //pointSize *= sin(time * length(gl_Color));


        // don't let points get below 1 pixel.
        //
        gl_PointSize = max(pointSize, 1.0);


        // when points would be smaller than 1 pixel, make them fade
        //
        vy_Color = gl_Color;
        vy_Color.a *= min(pointSize, 1.0);
      }
    )";
  }

  std::string fragmentCode() {
    return R"(
      varying vec4 vy_Color;
      void main() {
        gl_FragColor = vy_Color;
      }
    )";
  }
};

int main(int argc, char* argv[]) {
  MyApp().start();
  return 0;
}
