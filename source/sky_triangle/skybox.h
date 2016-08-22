#pragma once

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols

#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class Skybox
{
public:
    Skybox();
    ~Skybox();

    void initialize();
    bool leadGeometry();
    bool loadShaders();
    bool loadTextures();

    void render(glm::tmat4x4<float, glm::highp> viewProjection, glm::vec3 eye);
    void execute();

protected:
    bool loadSkyboxShader();
    void loadUniformLocations();

protected:
    // Skybox

    gl::GLuint m_skyboxVertices;
    gl::GLuint m_skyboxProgram;
    gl::GLuint m_skyboxVertexShader;
    gl::GLuint m_skyboxFragmentShader;
    gl::GLuint m_skyboxVAO;
    gl::GLuint m_skyboxTexture;

    // Uniforms

    gl::GLint m_skyboxLocation;
    gl::GLint m_skyboxProgramEyeLocation;
    gl::GLint m_transformMatrixLocation;

};
