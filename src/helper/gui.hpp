#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "platform/opengl.hpp"
#include "platform/window.hpp"

// A helper class for ImGui
class GUI {
public:
    // Sets up ImGui
    // @param window The current window
    static void setup(Window& window);
    // Cleans up ImGui
    static void cleanup();

    // Starts an ImGui frame
    static void newFrame();
    // Ends an ImGui frame
    static void endFrame();

    // Sets the current mouse mode
    // @param capture Sets if the mouse is captured by the window
    static void captureMouse(bool capture);

    /* Gets the current mouse position
     * @param xPos A reference to store the current x position of the mouse
     * @param yPos A reference to store the current y position of the mouse
    */
    static void getCursorPos(double& xPos, double& yPos);

    /* Determins if a key is pressed down
     * @param key The key to check
     * @returns Returns true if the key is being pressed
    */
    static bool isKeyDown(int key);

    /* Determins if a mouse button is pressed down
     * @param button The button to check
     * @returns Returns true if the button is being pressed
    */
    static bool isButtonDown(int button);
};
