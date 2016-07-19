
#include <chrono>

#include <glm/vec2.hpp>

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class e3task2
{
public:
    e3task2();
    ~e3task2();

    void initialize();
    bool leadGeometry();
    bool loadShaders();
    bool loadTextures();

    void resize(int w, int h);
    void render();
    void execute();

protected:
    bool loadModelShader();
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

    // Model

    gl::GLuint m_modelVertices;
    gl::GLuint m_modelNormals;
    gl::GLuint m_modelProgram;
    gl::GLuint m_modelVertexShader;
    gl::GLuint m_modelFragmentShader;
    gl::GLuint m_modelVAO;

    int m_modelVertexCount;

    // Uniforms

    gl::GLint m_skyboxLocation;
    gl::GLint m_cubemapLocation;
    gl::GLint m_modelProgramModelLocation;
    gl::GLint m_modelProgramNormalLocation;
    gl::GLint m_modelProgramViewProjectionLocation;
    gl::GLint m_modelProgramEyeLocation;
    gl::GLint m_transformMatrixLocation;

    // Canvas

    int m_width;
    int m_height;

    std::chrono::high_resolution_clock::time_point m_start;
};
