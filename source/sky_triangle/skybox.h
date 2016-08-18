

#include <glm/vec2.hpp>

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols

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
    bool loadModelShader();
    bool loadSkyboxShader();
    void loadUniformLocations();
    
    void loadCubeModel();

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
    gl::GLuint m_modelIndices;
    gl::GLuint m_modelProgram;
    gl::GLuint m_modelVertexShader;
    gl::GLuint m_modelGeometryShader;
    gl::GLuint m_modelFragmentShader;
    gl::GLuint m_modelVAO;
    gl::GLuint m_modelTextures[3];

    int m_modelVertexCount;

    // Uniforms

    gl::GLint m_skyboxLocation;
    gl::GLint m_cubemapLocation;
    gl::GLint m_modelProgramModelLocation;
    gl::GLint m_modelProgramViewProjectionLocation;
    gl::GLint m_modelProgramEyeLocation;
    gl::GLint m_transformMatrixLocation;
    
    gl::GLint a_vertex;
    gl::GLint m_modelVieProjectionLocation;

};
