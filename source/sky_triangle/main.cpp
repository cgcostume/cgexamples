
#include <iostream>

// C++ library for creating windows with OpenGL contexts and receiving 
// input and events http://www.glfw.org/ 
#include <GLFW/glfw3.h> 

// C++ binding for the OpenGL API. 
// https://github.com/cginternals/glbinding
#include <glbinding/Binding.h>

#include <cgutils/common.h>

#include "skytriangle.h"
#include "e3task2.h"


// From http://en.cppreference.com/w/cpp/language/namespace:
// "Unnamed namespace definition. Its members have potential scope 
// from their point of declaration to the end of the translation
// unit, and have internal linkage."
namespace
{

unsigned char renderMode = 0;
bool renderModeChanged = true;
bool lbutton_down = false;
double mousePosX, mousePosY;
glm::vec2 mouseSpeed;
const glm::vec2 minMouseSpeed = glm::vec2(2.5f);

auto example1 = SkyTriangle();
auto example2 = e3task2();

const auto canvasWidth = 1440; // in pixel
const auto canvasHeight = 900; // in pixel

// "The size callback ... which is called when the window is resized."
// http://www.glfw.org/docs/latest/group__window.html#gaa40cd24840daa8c62f36cafc847c72b6
void resizeCallback(GLFWwindow * /*window*/, int width, int height)
{
    example1.resize(width, height);
    example2.resize(width, height);
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
        example1.loadShaders();
        example2.loadShaders();
        break;
    case GLFW_KEY_V:
        renderMode = (++renderMode) % 3;
        renderModeChanged = true;
        break;
    }
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (GLFW_PRESS == action)
        {
            lbutton_down = true;
            glfwGetCursorPos(window, &mousePosX, &mousePosY);
        }
        else if (GLFW_RELEASE == action)
            lbutton_down = false;
    }
}

void getMouseSpeed(GLFWwindow* window)
{
    // Mouse dragging
    if (lbutton_down) {
        double currentMouseX, currentMouseY;
        glfwGetCursorPos(window, &currentMouseX, &currentMouseY);
        if (mousePosX != currentMouseX || mousePosY != currentMouseY)
        {
            mouseSpeed = glm::vec2(currentMouseX, currentMouseY) - glm::vec2(mousePosX, mousePosY);
            mousePosX = currentMouseX;
            mousePosY = currentMouseY;
        }
        else
        {
            mouseSpeed = glm::vec2(0.0);
        }
    }
    else
    {
        mouseSpeed = (mouseSpeed.x > 0.0) ? glm::max(minMouseSpeed, mouseSpeed - glm::vec2(0.1)) : glm::min(-minMouseSpeed, mouseSpeed + glm::vec2(0.1));
    }
}


// "In case a GLFW function fails, an error is reported to the GLFW 
// error callback. You can receive these reports with an error
// callback." http://www.glfw.org/docs/latest/quick.html#quick_capture_error
void errorCallback(int errnum, const char * errmsg)
{
    std::cerr << errnum << ": " << errmsg << std::endl;
}

}

void render()
{
    if(renderModeChanged)
    {
        renderModeChanged = false;
        static const auto modes = std::array<std::string, 3>{
            "(0) rendering with cubemap : ",
            "(1) environment with screen aligned triangle: ",
            "(2) both combined. left cubemap, right screen aligned triangle: "};
        std::cout << modes[renderMode] << std::endl;
    }
    
    switch (renderMode)
    {
    case 0:
        example1.render(mouseSpeed);
        break;
    case 1:
        example2.render();
        break;
    case 2:
        gl::glScissor(0, 0, canvasWidth/2, canvasHeight);
        glEnable(gl::GLenum::GL_SCISSOR_TEST);
        example1.render(mouseSpeed);

        gl::glScissor(canvasWidth / 2, 0, canvasWidth / 2, canvasHeight);
        example2.render();
        glDisable(gl::GLenum::GL_SCISSOR_TEST);
        break;
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(canvasWidth, canvasHeight, "", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 2;
    }

    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    std::cout << "Sky Triangle (no Skybox)" << std::endl << std::endl;

    std::cout << "Key Binding: " << std::endl
        << "  [F5] reload shaders" << std::endl
        << "  [v] switch draw mode" << std::endl
        << std::endl;

    glfwMakeContextCurrent(window);

    glbinding::Binding::initialize(false);

    example1.resize(canvasWidth, canvasHeight);
    example1.initialize();

    example2.resize(canvasWidth, canvasHeight);
    example2.initialize();

    while (!glfwWindowShouldClose(window)) // main loop
    {
        glfwPollEvents();

        getMouseSpeed(window);

        render();

        glfwSwapBuffers(window);
    }

    example1.cleanup();
    //example2.cleanup(); // TODO: implement cleanup function

    glfwMakeContextCurrent(nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
