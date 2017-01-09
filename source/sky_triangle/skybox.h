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
    gl::GLuint renderToTexture(glm::tmat4x4<float, glm::highp> viewProjection, glm::vec3 eye);
    void execute();
    
    void resize(int width, int height);

protected:
    bool loadSkyboxShader();
    bool loadOutputShader();
    void loadUniformLocations();
    bool loadFramebuffer();

protected:
    
    // Skybox
    gl::GLuint m_skyboxVertices;
    gl::GLuint m_skyboxProgram;
    gl::GLuint m_skyboxVertexShader;
    gl::GLuint m_skyboxFragmentShader;
    gl::GLuint m_skyboxVAO;
    gl::GLuint m_skyboxTexture;
    
    // Texture Output
    gl::GLuint m_fbo;
    gl::GLuint m_outputProgram;
    gl::GLuint m_outputVertexShader;
    gl::GLuint m_outputFragmentShader;
    gl::GLuint m_outputTexture;

    // Uniforms
    gl::GLint m_skyboxLocation;
    gl::GLint m_skyboxProgramEyeLocation;
    gl::GLint m_transformMatrixLocation;
    
    gl::GLint m_SkyboxLocationTex;
    gl::GLint m_SkyboxProgramEyeLocationTex;
    gl::GLint m_TransformMatrixLocationTex;
    
    int m_width;
    int m_height;

};
