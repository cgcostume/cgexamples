#pragma once

#include <string>
#include <vector>

#include <glbinding/gl/types.h>

#include <glm/gtc/matrix_transform.hpp>


class Cube
{
public:
    Cube();
    ~Cube();

    void initialize();
    void render(glm::tmat4x4<float, glm::highp> viewProjection);

    void setNumCubes(int numCubes);
    int numCubes() const;

protected:
    gl::GLint a_vertex;
    gl::GLint u_transform;
    gl::GLint u_numcubes;

    gl::GLuint m_vao;
    gl::GLuint m_indices;
    gl::GLuint m_vertices;

    gl::GLuint m_program;

    gl::GLuint m_textures[3];
    
    float m_a;
    int m_numcubes;
    std::vector<std::string> m_texturePaths;
};
