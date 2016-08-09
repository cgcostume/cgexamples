
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
using msecs = std::chrono::milliseconds;
struct Cursor
{
    bool lMouseButtonDown = false;
    glm::dvec2 position;
    glm::vec2 speed;
    const glm::vec2 minSpeed = glm::vec2(0.5f);
    glm::vec2 dragStartSpeed;
    std::chrono::time_point<std::chrono::high_resolution_clock> dragStart = std::chrono::high_resolution_clock::now();

    void updateDragSpeed(GLFWwindow* window, float time)
    {
        // Mouse dragging
        if (lMouseButtonDown) {
            glm::dvec2 currentCursorPos;
            glfwGetCursorPos(window, &currentCursorPos.x, &currentCursorPos.y);
            if (position != currentCursorPos)
            {
                speed = (currentCursorPos - position) * 0.2;
                dragStartSpeed = speed;
                dragStart = std::chrono::high_resolution_clock::now();
                position = currentCursorPos;
            }
            else
            {
                speed = glm::vec2(0.0);
            }
        }
        else
        {
            auto dragElapsed = static_cast<float>(std::chrono::duration_cast<msecs>(std::chrono::high_resolution_clock::now() - dragStart).count());
            dragElapsed *= 0.001f; // time is now in seconds

            auto decrease = dragStartSpeed / (1 + dragElapsed * dragElapsed * dragElapsed);
            speed = (speed.x > 0.0) ? glm::max(minSpeed, decrease) : glm::min(-minSpeed, decrease);
        }
    }
};
struct RenderMode
{
    unsigned char value = 0;
    bool changed = true;
    bool rotate;
    float angle = 0.f;
};

std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint = std::chrono::high_resolution_clock::now();

auto example1 = SkyTriangle();
auto example2 = e3task2();
    
RenderMode renderMode;
Cursor cursor;

const auto canvasWidth = 1440; // in pixel
const auto canvasHeight = 900; // in pixel
int frameBufferWidth, frameBufferHeight;

// "The size callback ... which is called when the window is resized."
// http://www.glfw.org/docs/latest/group__window.html#gaa40cd24840daa8c62f36cafc847c72b6
void resizeCallback(GLFWwindow * window, int width, int height)
{
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
    example1.resize(frameBufferWidth, frameBufferHeight);
    example2.resize(frameBufferWidth, frameBufferHeight);
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
        renderMode.value = (++renderMode.value) % 3;
        renderMode.changed = true;
        break;
    case GLFW_KEY_R:
        renderMode.rotate = !renderMode.rotate;
        break;
    }
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (GLFW_PRESS == action)
        {
            cursor.lMouseButtonDown = true;
            glfwGetCursorPos(window, &cursor.position.x, &cursor.position.y);
        }
        else if (GLFW_RELEASE == action)
            cursor.lMouseButtonDown = false;
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

void render(float time)
{
    if(renderMode.changed)
    {
        renderMode.changed = false;
        static const auto modes = std::array<std::string, 3>{
            "(0) environment with screen aligned triangle: ",
            "(1) rendering with cubemap: ",
            "(2) both combined. left screen aligned triangle, right cubemap: "};
        std::cout << modes[renderMode.value] << std::endl;
        startTimePoint = std::chrono::high_resolution_clock::now();
    }
    
    if(renderMode.rotate)
        renderMode.angle += cursor.speed.x ; // mouse dragging

    
    switch (renderMode.value)
    {
    case 0:
        example1.render(renderMode.angle);
        break;
    case 1:
        example2.render(time, renderMode.angle);
        break;
    case 2:
        gl::glScissor(0, 0, frameBufferWidth/2, frameBufferHeight);
        glEnable(gl::GLenum::GL_SCISSOR_TEST);
        example1.render(renderMode.angle);

        gl::glScissor(frameBufferWidth / 2, 0, frameBufferWidth / 2, frameBufferHeight);
        example2.render(time, renderMode.angle);

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

    std::cout << "Sky Triangle (no Skybox)" << std::endl << std::endl;

    std::cout << "Key Binding: " << std::endl
        << "  [F5] reload shaders" << std::endl
        << "  [v] switch draw mode" << std::endl
        << "  [r] toggle rotation" << std::endl
        << std::endl;

    glfwMakeContextCurrent(window);

    glbinding::Binding::initialize(false);

    example1.resize(frameBufferWidth, frameBufferHeight);
    example1.initialize();

    example2.resize(frameBufferWidth, frameBufferHeight);
    example2.initialize();

    while (!glfwWindowShouldClose(window)) // main loop
    {
        glfwPollEvents();

        const auto now = std::chrono::high_resolution_clock::now();
        auto time = static_cast<float>(std::chrono::duration_cast<msecs>(now - startTimePoint).count());
        time *= 0.001f; // time is now in seconds

        cursor.updateDragSpeed(window, time);

        render(time);

        glfwSwapBuffers(window);
    }

    example1.cleanup();

    glfwMakeContextCurrent(nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
