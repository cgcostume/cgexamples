
#include "scene.h"
#include "cursor.h"

#include <iostream>

// C++ library for creating windows with OpenGL contexts and receiving 
// input and events http://www.glfw.org/ 
#include <GLFW/glfw3.h> 

// C++ binding for the OpenGL API. 
// https://github.com/cginternals/glbinding
#include <glbinding/Binding.h>


// From http://en.cppreference.com/w/cpp/language/namespace:
// "Unnamed namespace definition. Its members have potential scope 
// from their point of declaration to the end of the translation
// unit, and have internal linkage."
namespace
{


Scene scene;
Cursor cursor;

const auto canvasWidth = 1440; // in pixel
const auto canvasHeight = 900; // in pixel
int frameBufferWidth, frameBufferHeight;

// "The size callback ... which is called when the window is resized."
// http://www.glfw.org/docs/latest/group__window.html#gaa40cd24840daa8c62f36cafc847c72b6
void resizeCallback(GLFWwindow * window, int width, int height)
{
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
    scene.resize(frameBufferWidth, frameBufferHeight);
}

// "The key callback ... which is called when a key is pressed, repeated or released."
// http://www.glfw.org/docs/latest/group__input.html#ga7e496507126f35ea72f01b2e6ef6d155
void keyCallback(GLFWwindow * /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (action != GLFW_RELEASE)
        return;

    switch (key)
    {
    case GLFW_KEY_F5:
        scene.loadShaders();
        break;
    case GLFW_KEY_V:
        scene.changeDrawMode();
        break;
    case GLFW_KEY_R:
        scene.toggleRotation();
        break;
    case GLFW_KEY_C:
        scene.changeCameraMode();
        break;
    case GLFW_KEY_DOWN:
        scene.changeRadiusBy(1.f);
        break;
    case GLFW_KEY_UP:
        scene.changeRadiusBy(-1.f);
        break;
    }
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (GLFW_PRESS == action)
        {
            cursor.lMouseButtonDown();
            glm::dvec2 position;
            glfwGetCursorPos(window, &position.x, &position.y);
            cursor.setPosition(position);
            
        }
        else if (GLFW_RELEASE == action)
            cursor.lMouseButtonUp();
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scene.changeRadiusBy(-yoffset);
}

// "In case a GLFW function fails, an error is reported to the GLFW 
// error callback. You can receive these reports with an error
// callback." http://www.glfw.org/docs/latest/quick.html#quick_capture_error
void errorCallback(int errnum, const char * errmsg)
{
    std::cerr << errnum << ": " << errmsg << std::endl;
}

}

int main(int /*argc*/, char ** /*argv*/)
{
    if (!glfwInit())
    {
        return 1;
    }

    glfwSetErrorCallback(errorCallback);

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(canvasWidth, canvasHeight, "", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 2;
    }
    
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    std::cout << "Sky Triangle (no Skybox)" << std::endl << std::endl;

    std::cout << "Key Binding: " << std::endl
        << "  [F5] reload shaders" << std::endl
        << "  [v] switch draw mode" << std::endl
        << "  [c] switch camera mode" << std::endl
        << "  [r] toggle rotation" << std::endl
        << "  [arrow up / scroll up] decrease radius" << std::endl
        << "  [arrow down / scroll down] increase radius" << std::endl
        << std::endl;

    glfwMakeContextCurrent(window);

    glbinding::Binding::initialize(false);

    scene.initialize();
    scene.resize(frameBufferWidth, frameBufferHeight);

    while (!glfwWindowShouldClose(window)) // main loop
    {
        glfwPollEvents();
        
        glm::dvec2 currentCursorPos;
        glfwGetCursorPos(window, &currentCursorPos.x, &currentCursorPos.y);
        cursor.updateDragSpeed(currentCursorPos);

        scene.render(cursor.speed().x);

        glfwSwapBuffers(window);
    }

    glfwMakeContextCurrent(nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
