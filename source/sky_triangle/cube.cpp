
#include "cube.h"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols

#include <cgutils/common.h>

using namespace gl;

namespace
{

}

Cube::Cube()
: a_vertex(-1)
, u_transform(-1)
, m_vao(0)
, m_indices(0)
, m_vertices(0)
, m_program(0)
, m_a(0.f)
, m_texturePaths{"patches.64.16.rgb.ub.raw", "patches.64.16.rgb.ub.raw", "patches.64.16.rgb.ub.raw"}
{
}

Cube::~Cube()
{
    glDeleteBuffers(1, &m_vertices);
    glDeleteBuffers(1, &m_indices);
    glDeleteProgram(m_program);
}

void Cube::initialize()
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    
    std::string vertexSource   = cgutils::textFromFile("data/sky_triangle/cube.vert");
    std::string geometrySource = cgutils::textFromFile("data/sky_triangle/cube.geom");
    std::string fragmentSource = cgutils::textFromFile("data/sky_triangle/cube.frag");
    
    const char * vertSource = vertexSource.c_str();
    const char * geomSource = geometrySource.c_str();
    const char * fragSource = fragmentSource.c_str();
    
    glShaderSource(vs, 1, &vertSource, nullptr);
    glCompileShader(vs);
    bool success = cgutils::checkForCompilationError(vs, "model vertex shader");
    
    glShaderSource(gs, 1, &geomSource, nullptr);
    glCompileShader(gs);
    success &= cgutils::checkForCompilationError(gs, "model geometry shader");
    
    
    glShaderSource(fs, 1, &fragSource, nullptr);
    glCompileShader(fs);
    success &= cgutils::checkForCompilationError(fs, "model fragment shader");
    
    m_program = glCreateProgram();
    
    glAttachShader(m_program, vs);
    glAttachShader(m_program, gs);
    glAttachShader(m_program, fs);
    
    glLinkProgram(m_program);
    success &= cgutils::checkForLinkerError(m_program, "model program");
    
    // create textures
    
    glGenTextures(3, m_textures);
    
    std::string dataPath = "data/sky_triangle/";
    for (int i = 0; i < 3; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        
        std::string texturePath = dataPath + m_texturePaths[i];
        {
            auto texture = cgutils::rawFromFile(texturePath.c_str());
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 64, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data()); // TODO: adapt to incoming texture
        }

    }
    
    
    // create cube
    
    static const GLfloat vertices_data[24] =
    {
        -1.f, -1.f, -1.f, // 0
        -1.f, -1.f,  1.f, // 1
        -1.f,  1.f, -1.f, // 2
        -1.f,  1.f,  1.f, // 3
        1.f, -1.f, -1.f, // 4
        1.f, -1.f,  1.f, // 5
        1.f,  1.f, -1.f, // 6
        1.f,  1.f,  1.f  // 7
    };
    
    static const GLubyte indices_data[18] = {
        2, 3, 6, 0, 1, 2, 1, 5, 3, 5, 4, 7, 4, 0, 6, 5, 1, 4 };
    
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    glGenBuffers(1, &m_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
    glBufferData(GL_ARRAY_BUFFER, (8 * 3) * sizeof(float), vertices_data, GL_STATIC_DRAW);
    
    glGenBuffers(1, &m_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (6 * 3) * sizeof(GLubyte), indices_data, GL_STATIC_DRAW);
    
    // setup uniforms
    
    a_vertex = glGetAttribLocation(m_program, "a_vertex");
    glEnableVertexAttribArray(static_cast<GLuint>(a_vertex));
    
    glVertexAttribPointer(static_cast<GLuint>(a_vertex), 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    u_transform = glGetUniformLocation(m_program, "modelViewProjection");
    
    GLint patches = glGetUniformLocation(m_program, "patches");
    
    
    glEnable(GL_DEPTH_TEST);
    
    glUseProgram(m_program);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glUniform1i(patches, 1);
}

void Cube::render(glm::tmat4x4<float, glm::highp> viewProjection)
{
    glUseProgram(m_program);
    glBindVertexArray(m_vao);

    auto modelMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.5f));
    
    
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(viewProjection * glm::translate(modelMatrix, glm::vec3(10.f, 0.f, 0.f))));
    glDrawElementsInstanced(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, 0, 1);
    
    glBindTexture(GL_TEXTURE_2D, m_textures[1]);
    glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(viewProjection * glm::translate(modelMatrix, glm::vec3(0.f, 10.f, 0.f))));
    glDrawElementsInstanced(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, 0, 1);
    
    glBindTexture(GL_TEXTURE_2D, m_textures[2]);
    glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(viewProjection * glm::translate(modelMatrix, glm::vec3(0.f, 0.f, 10.f))));
    glDrawElementsInstanced(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, 0, 1);
}
