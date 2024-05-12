#pragma once

#include "platform/opengl.hpp"
#include <glm/glm.hpp>

#include <string>
#include <memory>

// A class to handle the OpenGL context and window
class Window {
private:
  /* Creates a window and an OpenGL 4.3 context
   * @param resX The window width
   * @param resY The window height
   * @param title The window title
   * @param extraDebug Enables OpenGL debug
   */
  Window(int resX, int resY, const std::string &title, bool extraDebug = false);

public:
  ~Window();

  // Updates GLFW and swaps OpenGL buffers
  void update();

  // Returns true if the window should close. For example, when the exit button
  // is pressed
  // @returns Returns true if the window should close
  bool shouldClose();

  // Makes the window visable/invisable to the user
  // @param show True shows the window
  void showWindow(bool show);

  // Returns the handle to the GLFW window
  // @returns Returns the handle to the GLFW window
  GLFWwindow *getWindow();

  // Sets the OpenGL clear color
  // @param clearColor The clear color to use
  void setClearColor(glm::vec3 clearColor);

  // Enables/disables the depth test
  // @param test True enables depth testing
  void setDepthTest(bool test);

  /* Creates a window and an OpenGL 4.3 context
   * @param resX The window width
   * @param resY The window height
   * @param title The window title
   * @param extraDebug Enables OpenGL debug
   */
  inline static auto Create(int resX, int resY, const std::string &title, bool extraDebug = false) {
    return std::shared_ptr<Window>(new Window{resX, resY, title, extraDebug});
  }

private:
  GLFWwindow *window;
};
