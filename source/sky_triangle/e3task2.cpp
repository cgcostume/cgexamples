
#include "e3task2.h"

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

const std::vector<glm::vec3> cubeVertices = {
    glm::vec3(-10.0, -10.0,  10.0),
    glm::vec3(10.0, -10.0,  10.0),
    glm::vec3(-10.0,  10.0,  10.0),
    glm::vec3(10.0,  10.0,  10.0),
    glm::vec3(-10.0, -10.0, -10.0),
    glm::vec3(10.0, -10.0, -10.0),
    glm::vec3(-10.0,  10.0, -10.0),
    glm::vec3(10.0,  10.0, -10.0)
};

const std::vector<glm::vec3> cubeData = {
    // degenerate triangle at start allows for reversing the cube triangle strip, in order to change the winding
    cubeVertices[1], cubeVertices[1], cubeVertices[0],
    cubeVertices[4], cubeVertices[2], cubeVertices[6],
    cubeVertices[7], cubeVertices[4], cubeVertices[5],
    cubeVertices[1], cubeVertices[7], cubeVertices[3],
    cubeVertices[2], cubeVertices[1], cubeVertices[0]
};

}


e3task2::e3task2()
{
}

e3task2::~e3task2()
{
    // Flag all aquired resources for deletion (hint: driver decides when to actually delete them; see: shared contexts)
    glDeleteBuffers(1, &m_skyboxVertices);
    glDeleteProgram(m_skyboxProgram);
    glDeleteShader(m_skyboxVertexShader);
    glDeleteShader(m_skyboxFragmentShader);
    glDeleteVertexArrays(1, &m_skyboxVAO);
    glDeleteTextures(1, &m_skyboxTexture);

    glDeleteBuffers(1, &m_modelVertices);
    glDeleteBuffers(1, &m_modelNormals);
    glDeleteProgram(m_modelProgram);
    glDeleteShader(m_modelVertexShader);
    glDeleteShader(m_modelFragmentShader);
    glDeleteVertexArrays(1, &m_modelVAO);
}

void e3task2::initialize()
{
    // set color used when clearing the frame buffer
    glClearColor(0.12f, 0.14f, 0.18f, 1.0f);

    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &m_skyboxVertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * cubeData.size(), cubeData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /*std::vector<glm::vec3> modelVertices;
    std::vector<glm::vec3> modelNormals;*/
    //loadVertices("e3task2/teapot2.obj", modelVertices, modelNormals);

    //m_modelVertexCount = modelVertices.size();

    /*glGenBuffers(1, &m_modelVertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_modelVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * modelVertices.size(), modelVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_modelNormals);
    glBindBuffer(GL_ARRAY_BUFFER, m_modelNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * modelNormals.size(), modelNormals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    // create a rendering program, holding vertex and fragment shader
    m_skyboxProgram = glCreateProgram();

    // create a vertex shader
    m_skyboxVertexShader = glCreateShader(GL_VERTEX_SHADER);

    // create a fragment shader
    m_skyboxFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // attach shaders to program
    glAttachShader(m_skyboxProgram, m_skyboxVertexShader);
    glAttachShader(m_skyboxProgram, m_skyboxFragmentShader);

    // create a rendering program, holding vertex and fragment shader
    m_modelProgram = glCreateProgram();

    // create a vertex shader
    m_modelVertexShader = glCreateShader(GL_VERTEX_SHADER);

    // create a fragment shader
    m_modelFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // attach shaders to program
    glAttachShader(m_modelProgram, m_modelVertexShader);
    glAttachShader(m_modelProgram, m_modelFragmentShader);

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

    // create vertex attribute configuration (the input to vertex shaders)
    glGenVertexArrays(1, &m_modelVAO);

    // bind it since we want to operate on it
    glBindVertexArray(m_modelVAO);

    // bind vertex buffer to bnding point "array buffer": needed for vertex attribute configuration
    glBindBuffer(GL_ARRAY_BUFFER, m_modelVertices);

    // configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
    // read three floats, use them for vertex shader input 0 and move 3 floats forward for the next vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // bind vertex buffer to bnding point "array buffer": needed for vertex attribute configuration
    glBindBuffer(GL_ARRAY_BUFFER, m_modelNormals);

    // configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
    // read three floats, use them for vertex shader input 0 and move 3 floats forward for the next vertex
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // enable previously configured vertex shader input
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // create a texture 
    glGenTextures(1, &m_skyboxTexture);

    loadTextures();

    // optional: bind the fragment shader output 0 to "out_color", which is 0 by default
    glBindFragDataLocation(m_skyboxProgram, 0, "out_color");
    glBindFragDataLocation(m_modelProgram, 0, "out_color");

    m_direction = glm::vec3(0.f, 0.f, -1.f);

}

bool e3task2::loadShaders()
{
    //loadModelShader();
    loadSkyboxShader();
    loadUniformLocations();

    return true;
}

bool e3task2::loadModelShader()
{
    // attach 1 source to vertex shader
    const auto vertexShaderSource = textFromFile("e3task2/e3task2_model.vert");
    const auto vertexShaderSource_ptr = vertexShaderSource.c_str();
    if(vertexShaderSource_ptr)
        glShaderSource(m_modelVertexShader, 1, &vertexShaderSource_ptr, 0);

    // compile vertex shader
    glCompileShader(m_modelVertexShader);

    bool success = checkForCompilationError(m_modelVertexShader, "model vertex shader");

    // attach 1 source to fragment shader
    const auto fragmentShaderSource = textFromFile("e3task2/e3task2_model.frag");
    const auto fragmentShaderSource_ptr = fragmentShaderSource.c_str();
    if(fragmentShaderSource_ptr)
        glShaderSource(m_modelFragmentShader, 1, &fragmentShaderSource_ptr, 0);

    // compile fragment shader
    glCompileShader(m_modelFragmentShader);

    success &= checkForCompilationError(m_modelFragmentShader, "model fragment shader");

    if (!success)
    {
        return false;
    }

    // link program (confer to standard program compilation process: [tokenizer, lexer,] compiler, linker)
    glLinkProgram(m_modelProgram);

    success &= checkForLinkerError(m_modelProgram, "model program");

    if (!success)
    {
        return false;
    }

    return true;
}

bool e3task2::loadSkyboxShader()
{
    // attach 1 source to vertex shader
    const auto vertexShaderSource = textFromFile("data/sky_triangle/e3task2_skybox.vert");
    const auto vertexShaderSource_ptr = vertexShaderSource.c_str();
    if(vertexShaderSource_ptr)
        glShaderSource(m_skyboxVertexShader, 1, &vertexShaderSource_ptr, 0);

    // compile vertex shader
    glCompileShader(m_skyboxVertexShader);

    bool success = checkForCompilationError(m_skyboxVertexShader, "skybox vertex shader");

    // attach 1 source to fragment shader
    const auto fragmentShaderSource = textFromFile("data/sky_triangle/e3task2_skybox.frag");
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

void e3task2::loadUniformLocations()
{
    m_skyboxLocation  = glGetUniformLocation(m_skyboxProgram, "skybox");
    m_transformMatrixLocation = glGetUniformLocation(m_skyboxProgram, "transform");
    m_cubemapLocation  = glGetUniformLocation(m_modelProgram, "cubemap");
    m_modelProgramModelLocation = glGetUniformLocation(m_modelProgram, "model");
    m_modelProgramNormalLocation = glGetUniformLocation(m_modelProgram, "normal");
    m_modelProgramViewProjectionLocation = glGetUniformLocation(m_modelProgram, "viewProjection");
    m_modelProgramEyeLocation = glGetUniformLocation(m_modelProgram, "eye");

    glUseProgram(m_skyboxProgram);
    glUniform1i(m_skyboxLocation, 0);
    glUniformMatrix4fv(m_transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

    glUseProgram(m_modelProgram);
    glUniform1i(m_cubemapLocation, 0);
    glUniformMatrix4fv(m_modelProgramModelLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glUniformMatrix4fv(m_modelProgramNormalLocation, 1, GL_FALSE, glm::value_ptr(glm::mat3(1)));
    glUniformMatrix4fv(m_modelProgramViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

    glUseProgram(0);
}

bool e3task2::loadTextures()
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

void e3task2::resize(int w, int h)
{
    m_width = w;
    m_height = h;
}

void e3task2::render(float time, float angle)
{
    // Define the area for the rasterizer that is used for the NDC mapping ([-1, 1]^2 x [0, 1])
    glViewport(0, 0, m_width, m_height);

    // clear offscreen-framebuffer color attachment (no depth attachment configured and thus omitting GL_DEPTH_BUFFER_BIT)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto modelMatrix = glm::rotate(glm::mat4(1.f), time * 0.11f, glm::vec3(0.5f, 0.0f, 1.0f));
    //modelMatrix = glm::rotate(modelMatrix, cos(time * 0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));

    auto origin = glm::vec3(glm::rotate(glm::mat4(1.f), -time, glm::vec3(0.f, 1.f, 0.f))
        * glm::vec4(0.f, 0.f, 4.f, 1.0));
        

    const auto view = glm::lookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(sin(angle), 0.f, cos(angle)), glm::vec3(0.f, 1.f, 0.f));
    const auto projection = glm::perspective(glm::radians(80.f), static_cast<float>(m_width) / m_height, 1.f, 20.f);
    auto viewProjection = projection * view;


    glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

    glUseProgram(m_modelProgram);

    glUniformMatrix4fv(m_modelProgramModelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix3fv(m_modelProgramNormalLocation, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverse(glm::transpose(modelMatrix)))));
    glUniformMatrix4fv(m_modelProgramViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3f(m_modelProgramEyeLocation, origin.x, origin.y, origin.z);

    glBindVertexArray(m_modelVAO);
    
    //glDisable(GL_CULL_FACE); // only if model is shown
    glCullFace(GL_CW);
    glDrawArrays(GL_TRIANGLES, 0, m_modelVertexCount);


    glUseProgram(m_skyboxProgram);

    glUniformMatrix4fv(m_transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));

    glBindVertexArray(m_skyboxVAO);

    glCullFace(GL_CCW);

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    glDepthMask(GL_TRUE);

    glDepthFunc(GL_LESS);


    glBindVertexArray(0);
}

void e3task2::execute()
{
    render(0.0f, 0.0f);
}
