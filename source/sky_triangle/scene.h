#include "skybox.h"
#include "skytriangle.h"
#include "CubeScape.h"

#include <chrono>

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols
#include <glm/detail/type_vec3.hpp>



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
    void render(float speed);

protected:

    int m_width;
    int m_height;
    float m_padding;

    SkyTriangle example1;
    Skybox example2;
    CubeScape model;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;

    enum class DrawMode { Skytriangle, Cubemap, Both };
    int m_drawMode = 0;
    bool m_drawModeChanged = true;
    enum class CameraMode { Centered, Orbit };
    int m_cameraMode = 0;
    bool m_rotate;
    float m_angle = 0.f;

    glm::vec3 m_eye;
    glm::vec3 m_direction;
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
};
