#include "scene.h"

#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(pop)

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::initialize()
{
    m_startTimePoint = std::chrono::high_resolution_clock::now();

    // set color used when clearing the frame buffer
    gl::glClearColor(0.12f, 0.14f, 0.18f, 1.0f);

    example1.initialize();
    example2.initialize();
    model.initialize();
    model.setNumCubes(1);
}

void Scene::loadShaders()
{
    example1.loadShaders();
    example2.loadShaders();
}

void Scene::resize(int w, int h)
{
    m_width = w;
    m_height = h;
    m_padding = m_width / 1000.f;
    
    model.resize(w, h);
}

void Scene::changeDrawMode()
{
    m_drawMode = (++m_drawMode) % 3;
    m_drawModeChanged = true;
}

void Scene::changeCameraMode()
{
    m_cameraMode = (++m_cameraMode) % 2;
}

void Scene::toggleRotation()
{
    m_rotate = !m_rotate;
}

void Scene::render(float speed)
{
    // Define the area for the rasterizer that is used for the NDC mapping ([-1, 1]^2 x [0, 1])
    gl::glViewport(0, 0, m_width, m_height);

    // clear offscreen-framebuffer color attachment (no depth attachment configured and thus omitting GL_DEPTH_BUFFER_BIT)
    gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

    if (m_drawModeChanged)
    {
        m_drawModeChanged = false;
        static const auto drawModes = std::array<std::string, 3>{
                "(0) environment with screen aligned triangle: ",
                "(1) rendering with cubemap: ",
                "(2) both combined. left screen aligned triangle, right cubemap: "};
        std::cout << drawModes[m_drawMode] << std::endl;
        m_startTimePoint = std::chrono::high_resolution_clock::now();
    }

    // update angle
    if (m_rotate)
        m_angle += speed;
    if (m_angle > 360.f) m_angle = 0.f;
    else if (m_angle < 0.f) m_angle = 360.f;
    
    auto modelMatrix = glm::mat4(1.f);

    // update camera
    switch (m_cameraMode)
    {
    case 0:
        m_eye = glm::vec3(0.0f);
        m_direction = glm::vec3(sin(glm::radians(m_angle)), 0.f, cos(glm::radians(m_angle)));
        break;
    case 1:
        m_eye = glm::vec3(sin(glm::radians(180.f + m_angle)) * 10.f, 0.0f, cos(glm::radians(180.f + m_angle)) * 10.f);
            //add 180 degress to the angle to look in the same direction as in centered mode
        m_direction = -m_eye;
        modelMatrix = glm::rotate(glm::mat4(1.f),  m_angle, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    }

    const auto view = glm::lookAt(m_eye, m_direction, m_up);
    const auto projection = glm::perspective(glm::radians(80.f), static_cast<float>(m_width) / m_height, 1.f, 20.f);
    const auto viewProjection = projection * view;

    // draw
    switch (m_drawMode)
    {
    case static_cast<int>(DrawMode::Skytriangle) :
        //example1.render(viewProjection, modelMatrix, m_eye);
        model.draw();
        break;
    case static_cast<int>(DrawMode::Cubemap) :
        example2.render(viewProjection, m_eye);
        model.draw();
        break;
    case static_cast<int>(DrawMode::Both) :
        gl::glScissor(0, 0, m_width / 2 - m_padding, m_height);
        glEnable(gl::GLenum::GL_SCISSOR_TEST);
        example1.render(viewProjection, modelMatrix, m_eye);

        gl::glScissor(m_width / 2 + m_padding, 0, m_width / 2 - m_padding, m_height);
        example2.render(viewProjection, m_eye);

        glDisable(gl::GLenum::GL_SCISSOR_TEST);
        model.draw();
        break;
    }
}
