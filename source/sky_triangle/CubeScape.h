
#pragma once

#include <glbinding/gl/types.h>

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

//#include "glutils.h"


class CubeScape
{
public:
    CubeScape();
    ~CubeScape();

    void initialize();
    void draw(glm::tmat4x4<float, glm::highp> viewProjection);

    void setNumCubes(int numCubes);
    int numCubes() const;

protected:
    gl::GLint a_vertex;
    gl::GLint u_transform;
    gl::GLint u_time;
    gl::GLint u_numcubes;

    gl::GLuint m_vao;
    gl::GLuint m_indices;
    gl::GLuint m_vertices;

    gl::GLuint m_program;

    gl::GLuint m_textures[2];
    
    float m_a;
    int m_numcubes;

    using clock = std::chrono::system_clock;
    clock::time_point m_time;
};
