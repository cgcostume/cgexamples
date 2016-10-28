#pragma once


#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols
#include <glm/detail/type_vec3.hpp>

#include "skybox.h"
#include "skytriangle.h"
#include "cube.h"


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class Scene
{
public:
    Scene();
    ~Scene();

    void initialize();
    void loadShaders();

    void resize(int w, int h);
    void changeDrawMode();
    void changeCameraMode();
    void toggleRotation();
    void toggleLine();
    void changeRadiusBy(float value);
    void render(float speed);

protected:
    
    int m_width;
    int m_height;
    const float m_defaultMedian;
    float m_median;

    SkyTriangle example1;
    Skybox example2;
    Cube model;

    enum class DrawMode { Skytriangle, Cubemap, Both, Difference };
    int m_drawMode;
    bool m_drawModeChanged;
    enum class CameraMode { Centered, Orbit };
    int m_cameraMode;
    bool m_cameraModeChanged;
    bool m_rotate;
    float m_angle;
    float m_radius;

    float m_nearPlane;
    float m_farPlane;

    glm::vec3 m_eye;
    glm::vec3 m_direction;
    glm::vec3 m_up;
    
};
