
#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols

#include <glm/gtc/type_ptr.hpp>


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class SkyTriangle
{
public:
    SkyTriangle();
    ~SkyTriangle();

    void initialize();
    void cleanup();
    bool loadShaders();
    bool loadTextures();

    void render(glm::tmat4x4<float> viewProjection, glm::vec3 eye);
    void renderDifference(glm::tmat4x4<float> viewProjection, glm::vec3 eye, gl::GLuint texture);
    void execute();
    

protected:
    void loadUniformLocations();

protected:
    std::array<gl::GLuint, 1> m_vbos;

    std::array<gl::GLuint, 2> m_programs;
    std::array<gl::GLuint, 2> m_vertexShaders;
    std::array<gl::GLuint, 2> m_fragmentShaders;

    std::array<gl::GLuint, 1> m_vaos;

    std::array<gl::GLuint, 1> m_textures;
    std::array<gl::GLuint, 3> m_uniformLocations;
    
    std::array<gl::GLuint, 4> m_diffUniformLocations;
    
};
