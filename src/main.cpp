/*
 * This demo demostrates order independend
 * transperency using linked lists. It uses
 * four render passes.
 *
 * The first pass renders
 * opaque transparency.
 *
 * The second counts the
 * number of transparent fragments that need to
 * be drawn. The ABuffer is then resized to a
 * size that can hold all the transparent data.
 *
 * The third pass then renders all the transparent
 * data.
 *
 * The fourth pass resolves all the transparent
 * fragments and combines them with the opaque
 * data for the final output
 *
 * For more information, see NVidia's and AMD's
 * papers on the technique.
 * NVidia:
 * https://on-demand.gputechconf.com/gtc/2014/presentations/S4385-order-independent-transparency-opengl.pdf
 * AMD: http://developer.amd.com/wordpress/media/2013/06/2041_final.pdf
 */

#include "platform/buffer.hpp"
#include "platform/framebuffer.hpp"
#include "platform/shader.hpp"
#include "platform/texture.hpp"
#include "platform/window.hpp"

#include "rendering/camera.hpp"
#include "rendering/light.hpp"
#include "rendering/model.hpp"
#include "rendering/transform.hpp"

#include "helper/gui.hpp"
#include "helper/log.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace glm;

// OPAQUE_SHADER(shaders) is much easier to understand
// than shaders[0]
#define OPAQUE_SHADER(s) s[0]
#define COUNT_SHADER(s) s[1]
#define TRANSPARENT_SHADER(s) s[2]
#define COMBINE_SHADER(s) s[3]

#define ABUFFER_COUNTER(a) ((BufferCounter *)a[0])
#define ABUFFER_HEAD(a) ((BufferStorage *)a[1])
#define ABUFFER_DATA(a) ((BufferStorage *)a[2])

#define SPONZA(m) m[0]
#define DRAGON(m) m[1]

#define QUAD_POS(q) q[0]

#define OPAQUE_COLOR(g) g[0]
#define OPAQUE_DEPTH(g) g[1]

#define FRAMEBUFFER_OPAQUE(f) f[0]

const unsigned int RES_X = 1280;
const unsigned int RES_Y = 720;

const vec3 ambient = vec3(0.05f, 0.05f, 0.05f);
const vec3 skyColor = vec3(0.812f, 0.992f, 1.0f);

/* Since the opaque and transparency geometry
 * is rendered in different passes, we need a
 * seperate pass to combine the two. The
 * transparency method also needs two passes,
 * so the second pass is combined with the
 * combine pass. This quad is fullscreen quad
 * and uses the combine shader
 */
const float quadPosition[] = {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0,
                              1.0,  -1.0, 1.0, 1.0,  -1.0, 1.0};

// If this is Windows and is a release build,
// we don't want a console
#if defined(_WIN32) && defined(IS_RELEASE)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
            int nShowCmd) {
#else
int main() {
#endif
#ifdef DEBUG_OPENGL
  Window window(RES_X, RES_Y, "OIT OpenGL 4.3", true);
#else
  Window window(RES_X, RES_Y, "OIT OpenGL 4.3");
#endif
  GUI::setup(window);

  array<Framebuffer *, 1> fBuffers;
  array<TextureRender *, 2> gBuffers;
  array<DefaultShader *, 4> shaders;

  BufferArray vao;
  array<BufferData *, 1> dBuffers;

  array<Buffer *, 3> aBuffers;

  array<Light, 3> lights;

  array<Model *, 2> models;

  // Create the resources for the opaque pass
  FRAMEBUFFER_OPAQUE(fBuffers) = new Framebuffer(RES_X, RES_Y);

  OPAQUE_COLOR(gBuffers) = new TextureRender(RES_X, RES_Y, GL_RGBA16F);
  OPAQUE_DEPTH(gBuffers) = new TextureRender(RES_X, RES_Y, GL_R32F);

  FRAMEBUFFER_OPAQUE(fBuffers)->bind();
  FRAMEBUFFER_OPAQUE(fBuffers)->attach(OPAQUE_COLOR(gBuffers));
  FRAMEBUFFER_OPAQUE(fBuffers)->attach(OPAQUE_DEPTH(gBuffers));

  if (!FRAMEBUFFER_OPAQUE(fBuffers)->build()) {
    critical("Could not create framebuffer\n");
  }

  // Create shaders
  OPAQUE_SHADER(shaders) = new DefaultShader("res/shaders/solid/shader.vert",
                                             "res/shaders/solid/shader.frag");

  COUNT_SHADER(shaders) = new DefaultShader(
      "res/shaders/solid/shader.vert", "res/shaders/transparent/count.frag");

  TRANSPARENT_SHADER(shaders) = new DefaultShader(
      "res/shaders/solid/shader.vert", "res/shaders/transparent/shader.frag");

  COMBINE_SHADER(shaders) = new DefaultShader(
      "res/shaders/combine/shader.vert", "res/shaders/combine/shader.frag");

  // Create resources for the transparency pass
  aBuffers[0] = new BufferCounter();
  aBuffers[1] = new BufferStorage();
  aBuffers[2] = new BufferStorage();

  ABUFFER_COUNTER(aBuffers)->bind();
  ABUFFER_COUNTER(aBuffers)->setLocation(0);

  ABUFFER_HEAD(aBuffers)->bind();
  ABUFFER_HEAD(aBuffers)->setLocation(1);
  ABUFFER_HEAD(aBuffers)->resize(RES_X * RES_Y * 4);

  ABUFFER_DATA(aBuffers)->bind();
  ABUFFER_DATA(aBuffers)->setLocation(2);

  // Setup lights
  lights[0].position = vec3(0.0f, -45.0f, 0.0f);
  lights[0].color = vec3(1.0f, 1.0f, 1.0f);
  lights[0].strength = 5.0f;

  lights[1].position = vec3(500.0f, -400.0f, 0.0f);
  lights[1].color = vec3(1.0, 1.0, 0.9);
  lights[1].strength = 400.0f;

  lights[2].position = vec3(-500.0f, -400.0f, 0.0f);
  lights[2].color = vec3(0.9, 1.0, 1.0);
  lights[2].strength = 400.0f;

  // Upload shader constants
  for (auto &shader : shaders) {
    shader->bind();

    if ((shader == TRANSPARENT_SHADER(shaders)) ||
        (shader == COMBINE_SHADER(shaders))) {
      shader->setUniformUInt("screenWidth", RES_X);
    }

    if ((shader == OPAQUE_SHADER(shaders)) ||
        (shader == TRANSPARENT_SHADER(shaders))) {
      shader->setUniformUInt("lightCount", lights.size());
      shader->setUniformVec3("globalAmbient", ambient);

      for (size_t i = 0; i < lights.size(); i++) {
        lights[i].set(shader, i);
      }
    }

    if ((shader == COUNT_SHADER(shaders)) ||
        (shader == TRANSPARENT_SHADER(shaders))) {
      shader->setUniformTexture("sceneDepth", 0);
    }
  }

  // Create resources for combine pass
  vao.bind();

  QUAD_POS(dBuffers) =
      new BufferData(sizeof(quadPosition), (void *)quadPosition);

  vao.setAttribute(0, 2, GL_FLOAT, 0, 0);

  // Load models and set transforms
  SPONZA(models) = new Model("res/sponza/sponza.obj", "res/sponza/");
  DRAGON(models) = new Model("res/dragon/dragon.obj", "res/dragon/");

  DRAGON(models)->transform.position = vec3(0.0f, -50.0f, 0.0f);
  DRAGON(models)->transform.scale = vec3(50.0f);

  // Create camera and set transform
  Camera camera(70.0f, RES_X / (float)RES_Y, 10.0f, 3000.0f);
  camera.transform.position.z = -75.0f;
  camera.transform.position.y = -50.0f;

  double currentTime = glfwGetTime();
  double lastTime = currentTime;
  double deltaTime;

  double lastPosX = 0.0;
  double lastPosY = 0.0;

  float mouseSpeed = 4.0f;
  float movementSpeed = 100.0f;

  bool showBackside = false;

  window.setClearColor(skyColor);

  // Loading is done. Show the window
  window.showWindow(true);
  while (!window.shouldClose()) {
    // Calculate delta time
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Rotate dragon
    DRAGON(models)->transform.rotateAxis(vec3(0.0f, 1.0f, 0.0f),
                                         -deltaTime * 3.0f);

    // Calculate camera movement if the right mouse button
    // is down
    if (GUI::isButtonDown(1)) {
      GUI::captureMouse(true);

      // Calculate mouse delta
      double currentPosX, currentPosY;
      GUI::getCursorPos(currentPosX, currentPosY);

      double deltaPosX = lastPosX - currentPosX;
      double deltaPosY = lastPosY - currentPosY;

      lastPosX = currentPosX;
      lastPosY = currentPosY;

      deltaPosX /= 16.0;
      deltaPosY /= 16.0;

      // Calculate rotation delta
      glm::vec3 cameraRotation =
          glm::vec3(radians(-deltaPosY * mouseSpeed),
                    radians(-deltaPosX * mouseSpeed * 0.666f), 0.0f);

      camera.rotation += cameraRotation;

      // Clamp camera rotation
      float PI = pi<float>();
      float PI_HALF = PI / 2.0f;

      if (camera.rotation.x < -PI_HALF) {
        camera.rotation.x = -PI_HALF;
      } else if (camera.rotation.x > PI_HALF) {
        camera.rotation.x = PI_HALF;
      }

      float speed = movementSpeed;

      // Get camera vectors
      glm::vec3 forward, right, up;

      forward = camera.getForward();
      right = camera.getRight();
      up = camera.getUp();

      // Calculate final camera speed
      if (GUI::isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        speed *= 1.5f;
      }

      // Calculate movement delta
      glm::vec3 deltaTranslate = glm::vec3(deltaTime * speed);

      if (GUI::isKeyDown(GLFW_KEY_W)) {
        camera.transform.position += forward * deltaTranslate;
      }

      if (GUI::isKeyDown(GLFW_KEY_S)) {
        camera.transform.position -= forward * deltaTranslate;
      }

      if (GUI::isKeyDown(GLFW_KEY_A)) {
        camera.transform.position += right * deltaTranslate;
      }

      if (GUI::isKeyDown(GLFW_KEY_D)) {
        camera.transform.position -= right * deltaTranslate;
      }

      if (GUI::isKeyDown(GLFW_KEY_E)) {
        camera.transform.position += up * deltaTranslate;
      }

      if (GUI::isKeyDown(GLFW_KEY_Q)) {
        camera.transform.position -= up * deltaTranslate;
      }
    } else {
      GUI::captureMouse(false);

      // Updated mouse position for delta calculations
      GUI::getCursorPos(lastPosX, lastPosY);
    }

    // Update shaders
    for (auto &shader : shaders) {
      shader->bind();

      if ((shader != COMBINE_SHADER(shaders))) {
        shader->setUniformMatrix("PV", camera.getMatrix());
      }

      if ((shader == OPAQUE_SHADER(shaders)) ||
          (shader == TRANSPARENT_SHADER(shaders))) {
        shader->setUniformVec3("viewPosition", camera.transform.position);
      }
    }

    // Opaque pass
    ABUFFER_COUNTER(aBuffers)->reset();

    FRAMEBUFFER_OPAQUE(fBuffers)->bind();
    FRAMEBUFFER_OPAQUE(fBuffers)->clear();

    SPONZA(models)->draw(OPAQUE_SHADER(shaders));

    // Count pass
    window.setDepthTest(false);
    OPAQUE_DEPTH(gBuffers)->bind(0);

    DRAGON(models)->draw(COUNT_SHADER(shaders));

    // Transparent pass

    // Resize transparency buffers to the number of
    // transparent fragments to draw
    ABUFFER_DATA(aBuffers)->resize(ABUFFER_COUNTER(aBuffers)->read() * 48);
    ABUFFER_COUNTER(aBuffers)->reset();
    ABUFFER_HEAD(aBuffers)->clear();

    DRAGON(models)->draw(TRANSPARENT_SHADER(shaders));

    // Combine pass
    window.setDepthTest(true);

    FRAMEBUFFER_OPAQUE(fBuffers)->unbind();

    COMBINE_SHADER(shaders)->bind();

    for (size_t i = 0; i < gBuffers.size(); i++) {
      gBuffers[i]->bind(i);
    }

    COMBINE_SHADER(shaders)->setUniformTexture("opaqueColor", 0);
    ABUFFER_DATA(aBuffers)->barrier();

    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    vao.unbind();

    // Draw the GUI
    GUI::newFrame();

    ImGui::Begin("Info");
    // Window resolution
    ImGui::Text("Resolution: (%d, %d)", RES_X, RES_Y);

    ImGui::Separator();

    // FPS display
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    ImGui::Separator();

    ImGui::DragFloat("Mouse Speed", &mouseSpeed, 0.4f, 0.1f, 10.0f, "%.3f",
                     1.0f);
    ImGui::DragFloat("Movement Speed", &movementSpeed, 0.2f, 100.0f, 300.0f,
                     "%.3f", 1.0f);

    ImGui::Separator();

    // Movement instructions
    ImGui::Text("WASD - Movement");
    ImGui::Text("EQ - Raise / Lower");
    ImGui::Text("Mouse - Move Camera");
    ImGui::Text("Left Shift - Increase Speed");
    ImGui::Text("Hold the right mouse button to move/look around");

    ImGui::End();
    GUI::endFrame();

    // Swap framebuffers
    window.update();
  }

  // Cleanup objects
  for (auto &model : models) {
    delete model;
  }

  for (auto &buffer : aBuffers) {
    delete buffer;
  }

  for (auto &buffer : dBuffers) {
    delete buffer;
  }

  for (auto &shader : shaders) {
    delete shader;
  }

  for (auto &texture : gBuffers) {
    delete texture;
  }

  for (auto &framebuffer : fBuffers) {
    delete framebuffer;
  }

  return 0;
}
