#pragma once

#include <chrono>
#include <glm/detail/type_vec2.hpp>

using msecs = std::chrono::milliseconds;
class Cursor
{
public:
    Cursor();
    ~Cursor();
    
    void lMouseButtonDown();
    void lMouseButtonUp();
    void setPosition(glm::dvec2 position);
    void updateDragSpeed(glm::dvec2 currentCursorPos);
    glm::vec2 speed();
    
protected:
    
    bool m_lMouseButtonDown = false;
    glm::dvec2 m_position;
    glm::vec2 m_speed;
    const glm::vec2 m_minSpeed = glm::vec2(0.4f);
    glm::vec2 m_dragStartSpeed;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_dragStart = std::chrono::high_resolution_clock::now();
    
    
};