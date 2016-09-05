
#include "Skybox.h"

#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(pop)

#include <glbinding/gl32core/gl.h>

#include <cgutils/common.h>

using namespace gl32core;
using namespace cgutils;


namespace
{

const std::vector<glm::vec3> envCubeVertices = {
    glm::vec3(-10.0, -10.0,  10.0),
    glm::vec3(10.0, -10.0,  10.0),
    glm::vec3(-10.0,  10.0,  10.0),
    glm::vec3(10.0,  10.0,  10.0),
    glm::vec3(-10.0, -10.0, -10.0),
    glm::vec3(10.0, -10.0, -10.0),
    glm::vec3(-10.0,  10.0, -10.0),
    glm::vec3(10.0,  10.0, -10.0)
};

const std::vector<glm::vec3> envCubeData = {
    // degenerate triangle at start allows for reversing the cube triangle strip, in order to change the winding
    envCubeVertices[1], envCubeVertices[1], envCubeVertices[0],
    envCubeVertices[4], envCubeVertices[2], envCubeVertices[6],
    envCubeVertices[7], envCubeVertices[4], envCubeVertices[5],
    envCubeVertices[1], envCubeVertices[7], envCubeVertices[3],
    envCubeVertices[2], envCubeVertices[1], envCubeVertices[0]
};

}


Skybox::Skybox()
{
}

Skybox::~Skybox()
{
    // Flag all aquired resources for deletion (hint: driver decides when to actually delete them; see: shared contexts)
    glDeleteBuffers(1, &m_skyboxVertices);
    glDeleteProgram(m_skyboxProgram);
    glDeleteShader(m_skyboxVertexShader);
    glDeleteShader(m_skyboxFragmentShader);
    glDeleteVertexArrays(1, &m_skyboxVAO);
    glDeleteTextures(1, &m_skyboxTexture);
}

void Skybox::initialize()
{
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &m_skyboxVertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * envCubeData.size(), envCubeData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create a rendering program, holding vertex and fragment shader
    m_skyboxProgram = glCreateProgram();

    // create a vertex shader
    m_skyboxVertexShader = glCreateShader(GL_VERTEX_SHADER);

    // create a fragment shader
    m_skyboxFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // attach shaders to program
    glAttachShader(m_skyboxProgram, m_skyboxVertexShader);
    glAttachShader(m_skyboxProgram, m_skyboxFragmentShader);

    loadShaders();

    // create vertex attribute configuration (the input to vertex shaders)
    glGenVertexArrays(1, &m_skyboxVAO);

    // bind it since we want to operate on it
    glBindVertexArray(m_skyboxVAO);

    // bind vertex buffer to bnding point "array buffer": needed for vertex attribute configuration
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVertices);

    // configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
    // read three floats, use them for vertex shader input 0 and move 3 floats forward for the next vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // enable previously configured vertex shader input
    glEnableVertexAttribArray(0);

    // create a texture 
    glGenTextures(1, &m_skyboxTexture);

    loadTextures();

    // optional: bind the fragment shader output 0 to "out_color", which is 0 by default
    glBindFragDataLocation(m_skyboxProgram, 0, "out_color");
}

bool Skybox::loadShaders()
{
    loadSkyboxShader();
    loadUniformLocations();

    return true;
}

bool Skybox::loadSkyboxShader()
{
    // attach 1 source to vertex shader
    const auto vertexShaderSource = textFromFile("data/sky_triangle/skybox.vert");
    const auto vertexShaderSource_ptr = vertexShaderSource.c_str();
    if(vertexShaderSource_ptr)
        glShaderSource(m_skyboxVertexShader, 1, &vertexShaderSource_ptr, 0);

    // compile vertex shader
    glCompileShader(m_skyboxVertexShader);

    bool success = checkForCompilationError(m_skyboxVertexShader, "skybox vertex shader");

    // attach 1 source to fragment shader
    const auto fragmentShaderSource = textFromFile("data/sky_triangle/skybox.frag");
    const auto fragmentShaderSource_ptr = fragmentShaderSource.c_str();
    if(fragmentShaderSource_ptr)
        glShaderSource(m_skyboxFragmentShader, 1, &fragmentShaderSource_ptr, 0);

    // compile fragment shader
    glCompileShader(m_skyboxFragmentShader);

    success &= checkForCompilationError(m_skyboxFragmentShader, "skybox fragment shader");

    if (!success)
    {
        return false;
    }

    // link program (confer to standard program compilation process: [tokenizer, lexer,] compiler, linker)
    glLinkProgram(m_skyboxProgram);

    success &= checkForLinkerError(m_skyboxProgram, "skybox program");

    if (!success)
    {
        return false;
    }

    return true;
}

void Skybox::loadUniformLocations()
{
    m_skyboxLocation  = glGetUniformLocation(m_skyboxProgram, "skybox");
    m_transformMatrixLocation = glGetUniformLocation(m_skyboxProgram, "transform");
    m_skyboxProgramEyeLocation = glGetUniformLocation(m_skyboxProgram, "eye");

    glUseProgram(m_skyboxProgram);
    glUniform1i(m_skyboxLocation, 0);
    glUniformMatrix4fv(m_transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

    glUseProgram(0);
}

bool Skybox::loadTextures()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

    auto rawPX = rawFromFile("data/sky_triangle/cube_gen_17_px.1024.1024.rgb.ub.raw");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, static_cast<GLint>(GL_RGB8), 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, rawPX.data());

    auto rawPY = rawFromFile("data/sky_triangle/cube_gen_17_py.1024.1024.rgb.ub.raw");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, static_cast<GLint>(GL_RGB8), 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, rawPY.data());

    auto rawPZ = rawFromFile("data/sky_triangle/cube_gen_17_pz.1024.1024.rgb.ub.raw");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, static_cast<GLint>(GL_RGB8), 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, rawPZ.data());

    auto rawNX = rawFromFile("data/sky_triangle/cube_gen_17_nx.1024.1024.rgb.ub.raw");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, static_cast<GLint>(GL_RGB8), 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, rawNX.data());

    auto rawNY = rawFromFile("data/sky_triangle/cube_gen_17_ny.1024.1024.rgb.ub.raw");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, static_cast<GLint>(GL_RGB8), 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, rawNY.data());

    auto rawNZ = rawFromFile("data/sky_triangle/cube_gen_17_nz.1024.1024.rgb.ub.raw");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, static_cast<GLint>(GL_RGB8), 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, rawNZ.data());
    
    // configure required min/mag filter and wrap modes
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));

    return true;
}

void Skybox::render(glm::tmat4x4<float, glm::highp> viewProjection, glm::vec3 eye)
{

    glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);


    //render skybox //////////////////////////////////////////////////////////////////////////////
    

    glUseProgram(m_skyboxProgram);

    glUniformMatrix4fv(m_transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3f(m_skyboxProgramEyeLocation, eye.x, eye.y, eye.z);

    glBindVertexArray(m_skyboxVAO);

    glCullFace(GL_CCW);

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    glDepthMask(GL_TRUE);

    glDepthFunc(GL_LESS);
    
    
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}


void Skybox::execute()
{
}
