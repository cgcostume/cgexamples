
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
    
    static const std::vector<GLfloat> modelVertices =
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

    glDeleteBuffers(1, &m_modelVertices);
    glDeleteProgram(m_modelProgram);
    glDeleteShader(m_modelVertexShader);
    glDeleteShader(m_modelGeometryShader);
    glDeleteShader(m_modelFragmentShader);
    glDeleteVertexArrays(1, &m_modelVAO);
}

void Skybox::initialize()
{
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &m_skyboxVertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * envCubeData.size(), envCubeData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_modelVertexCount = modelVertices.size();

//    glGenBuffers(1, &m_modelVertices);
//    glBindBuffer(GL_ARRAY_BUFFER, m_modelVertices);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * modelVertices.size(), modelVertices.data(), GL_STATIC_DRAW);

    loadCubeModel();

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
    
    // create a geometry shader
    m_modelGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    // create a fragment shader
    m_modelFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // attach shaders to program
    glAttachShader(m_modelProgram, m_modelVertexShader);
    glAttachShader(m_modelProgram, m_modelGeometryShader);
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
    //glGenVertexArrays(1, &m_modelVAO);

    // bind it since we want to operate on it
    //glBindVertexArray(m_modelVAO);

    // bind vertex buffer to bnding point "array buffer": needed for vertex attribute configuration
    //glBindBuffer(GL_ARRAY_BUFFER, m_modelVertices);

    // configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
    // read three floats, use them for vertex shader input 0 and move 3 floats forward for the next vertex
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
    // read three floats, use them for vertex shader input 0 and move 3 floats forward for the next vertex
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // enable previously configured vertex shader input
    glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);

    // create a texture 
    glGenTextures(1, &m_skyboxTexture);

    loadTextures();

    // optional: bind the fragment shader output 0 to "out_color", which is 0 by default
    glBindFragDataLocation(m_skyboxProgram, 0, "out_color");
    glBindFragDataLocation(m_modelProgram, 0, "out_color");
}

void Skybox::loadCubeModel()
{
    
    glGenVertexArrays(1, &m_modelVAO);
    glBindVertexArray(m_modelVAO);
    
    glGenBuffers(1, &m_modelVertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_modelVertices);
    glBufferData(GL_ARRAY_BUFFER, (8 * 3) * sizeof(float), modelVertices.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &m_modelIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (6 * 3) * sizeof(GLubyte), indices_data, GL_STATIC_DRAW);
    
    a_vertex = glGetAttribLocation(m_modelProgram, "a_vertex");
        
    glVertexAttribPointer(static_cast<GLuint>(a_vertex), 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(static_cast<GLuint>(a_vertex));

}

bool Skybox::loadShaders()
{
    loadModelShader();
    loadSkyboxShader();
    loadUniformLocations();

    return true;
}

bool Skybox::loadModelShader()
{
    // attach 1 source to vertex shader
    const auto vertexShaderSource = textFromFile("data/sky_triangle/skybox_model.vert");
    const auto vertexShaderSource_ptr = vertexShaderSource.c_str();
    if(vertexShaderSource_ptr)
        glShaderSource(m_modelVertexShader, 1, &vertexShaderSource_ptr, 0);

    // compile vertex shader
    glCompileShader(m_modelVertexShader);

    bool success = checkForCompilationError(m_modelVertexShader, "model vertex shader");
    
    // attach 1 source to geometry shader
    const auto geometryShaderSource = textFromFile("data/sky_triangle/skybox_model.geom");
    const auto geometryShaderSource_ptr = geometryShaderSource.c_str();
    if(geometryShaderSource_ptr)
        glShaderSource(m_modelGeometryShader, 1, &geometryShaderSource_ptr, 0);
    
    // compile geometry shader
    glCompileShader(m_modelGeometryShader);
    
    success &= checkForCompilationError(m_modelGeometryShader, "model geometry shader");


    // attach 1 source to fragment shader
    const auto fragmentShaderSource = textFromFile("data/sky_triangle/skybox_model.frag");
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
    
    m_cubemapLocation  = glGetUniformLocation(m_modelProgram, "cubemap");
    m_modelProgramModelLocation = glGetUniformLocation(m_modelProgram, "model");
    m_modelProgramViewProjectionLocation = glGetUniformLocation(m_modelProgram, "viewProjection");
    m_modelProgramEyeLocation = glGetUniformLocation(m_modelProgram, "eye");
    
    
    m_modelVieProjectionLocation = glGetUniformLocation(m_modelProgram, "modelViewProjection");
    GLint patches = glGetUniformLocation(m_modelProgram, "patches");
    GLint terrain = glGetUniformLocation(m_modelProgram, "terrain");

    glUseProgram(m_skyboxProgram);
    glUniform1i(m_skyboxLocation, 0);
    glUniformMatrix4fv(m_transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

    glUseProgram(m_modelProgram);
    glUniformMatrix4fv(m_modelProgramModelLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glUniformMatrix4fv(m_modelProgramViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_modelTextures[0]);
    glUniform1i(patches, 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_modelTextures[1]);
    glUniform1i(terrain, 2);

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
    
    // create textures
    
    glGenTextures(3, m_modelTextures);
    
    glBindTexture(GL_TEXTURE_2D, m_modelTextures[0]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    {
        auto modelTex = rawFromFile("data/sky_triangle/patches.64.16.rgb.ub.raw");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 64, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, modelTex.data());
    }

    
    glBindTexture(GL_TEXTURE_2D, m_modelTextures[1]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    {
        auto terrainTex = rawFromFile("data/sky_triangle/terrain.64.64.r.ub.raw");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 64, 64, 0, GL_RED, GL_UNSIGNED_BYTE, terrainTex.data());
    }


    return true;
}

void Skybox::render(glm::tmat4x4<float, glm::highp> viewProjection, glm::vec3 eye)
{

    //auto modelMatrix = glm::rotate(glm::mat4(1.f), time * 0.11f, glm::vec3(0.5f, 0.0f, 1.0f));
    //modelMatrix = glm::rotate(modelMatrix, cos(time * 0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
    
    auto modelMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f));

    /*auto origin = glm::vec3(glm::rotate(glm::mat4(1.f), -time, glm::vec3(0.f, 1.f, 0.f))
        * glm::vec4(0.f, 0.f, 4.f, 1.0));*/

    glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);


    //render skybox //////////////////////////////////////////////////////////////////////////////
    

    glUseProgram(m_skyboxProgram);

    glUniformMatrix4fv(m_transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3f(m_modelProgramEyeLocation, eye.x, eye.x, eye.z);

    glBindVertexArray(m_skyboxVAO);

    glCullFace(GL_CCW);

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
    glDepthMask(GL_TRUE);

    glDepthFunc(GL_LESS);
    
    
    
    //render model //////////////////////////////////////////////////////////////////////////////
    
    glUseProgram(m_modelProgram);
    
    glUniformMatrix4fv(m_modelProgramModelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    //glUniformMatrix3fv(m_modelProgramNormalLocation, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverse(glm::transpose(modelMatrix)))));
    glUniformMatrix4fv(m_modelProgramViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniformMatrix4fv(m_modelVieProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection * modelMatrix));
    //glUniform3f(m_modelProgramEyeLocation, origin.x, origin.y, origin.z);
    
    glBindVertexArray(m_modelVAO);
    
    //glDisable(GL_CULL_FACE); // only if model is shown
    //glCullFace(GL_CW);
    
    glEnable(GL_DEPTH_TEST);
    //glDrawArrays(GL_TRIANGLES, 0, m_modelVertexCount);
    //glDrawElementsInstanced(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, 0, 100);

    glBindVertexArray(0);
}


void Skybox::execute()
{
}
