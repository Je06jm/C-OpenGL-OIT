#include "window.hpp"

#include "helper/log.hpp"

#include <stdio.h>

using namespace std;

void OpenGLMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    if (type == GL_DEBUG_TYPE_ERROR) {
        // OpenGL error messages
        critical(
            "OpenGL Error:\n"
            "\tSource: %u\n"
            "\tType: %u\n"
            "\tID: %u\n"
            "\tSeverity: %u\n"
            "\tMessage: %s\n",
            source,
            type,
            id,
            severity,
            message
        );
    } else {
        // Every other type of OpenGL message
        info(
            "OpenGL Message:\n"
            "\tSource: %u\n"
            "\tType: %u\n"
            "\tID: %u\n"
            "\tSeverity: %u\n"
            "\tMessage: %s\n",
            source,
            type,
            id,
            severity,
            message
        );
    }
}

Window::Window(int resX, int resY, const string& title, bool extraDebug) {
    glfwInit();

    // Tell GLFW about what type of OpenGL context we want to create.
    // Also informs GLFW about the type of window we want to create
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    // Creates the window and sets the OpenGL context as the current
    // context
    this->window = glfwCreateWindow(resX, resY, title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(this->window);

    // OpenGL function loader
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        critical("Failed to initialize GLAD\n");
    }

    if (extraDebug) {
        // Tells OpenGL that we want debugging info
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGLMessage, nullptr);
    }

    // Sets up OpenGL to a known state
    glViewport(0, 0, resX, resY);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

Window::~Window() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Window::update() {
    glfwPollEvents();
    glfwSwapBuffers(this->window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(this->window);
}

void Window::showWindow(bool show) {
    if (show){
        glfwShowWindow(this->window);
    } else {
        glfwHideWindow(this->window);
    }
}

GLFWwindow* Window::getWindow() {
    return this->window;
}

void Window::setClearColor(glm::vec3 clearColor) {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
}

void Window::setDepthTest(bool test) {
    if (test) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }
}
