#include "helper/gui.hpp"

GLFWwindow* glfwWindow = nullptr;

void GUI::setup(Window& window) {
    glfwWindow = window.getWindow();

    // Sets up ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    // Inits ImGui
    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init();
}

void GUI::cleanup() {
    // Cleans up ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::newFrame() {
    // Creates a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::endFrame() {
    // Ends a ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::captureMouse(bool capture) {
    // Sets the mouse mode to either GLFW_CURSOR_DISABLED or GLFW_CURSOR_NORMAL
    if (capture) {
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void GUI::getCursorPos(double& xPos, double& yPos) {
    glfwGetCursorPos(glfwWindow, &xPos, &yPos);
}

bool GUI::isKeyDown(int key) {
    return glfwGetKey(glfwWindow, key) == GLFW_PRESS;
}

bool GUI::isButtonDown(int button) {
    return glfwGetMouseButton(glfwWindow, button) == GLFW_PRESS;
}
