#include "cursor.h"

#include <glm/glm.hpp>

Cursor::Cursor()
: m_lMouseButtonDown(false)
, m_position()
, m_speed()
, m_minSpeed(0.4f)
, m_dragStartSpeed()
, m_dragStart(std::chrono::high_resolution_clock::now())
{
}

Cursor::~Cursor(){}

void Cursor::lMouseButtonDown()
{
    m_lMouseButtonDown = true;
}

void Cursor::lMouseButtonUp()
{
    m_lMouseButtonDown = false;
}

void Cursor::setPosition(glm::dvec2 position)
{
    m_position = position;
}

glm::vec2 Cursor::speed()
{
    return m_speed;
}

void Cursor::updateDragSpeed(glm::dvec2 currentCursorPos)
{
    // Mouse dragging
    if (m_lMouseButtonDown) {
        if (m_position != currentCursorPos)
        {
            m_speed = (currentCursorPos - m_position) * 0.2;
            m_dragStartSpeed = m_speed;
            m_dragStart = std::chrono::high_resolution_clock::now();
            m_position = currentCursorPos;
        }
        else
        {
            m_speed = glm::vec2(0.0);
        }
    }
    else
    {
        auto dragElapsed = static_cast<float>(std::chrono::duration_cast<msecs>(std::chrono::high_resolution_clock::now() - m_dragStart).count());
        dragElapsed *= 0.001f; // time is now in seconds
        
        auto decrease = m_dragStartSpeed / (1 + dragElapsed * dragElapsed * dragElapsed);
        m_speed = (m_speed.x > 0.0) ? glm::max(m_minSpeed, decrease) : glm::min(-m_minSpeed, decrease);
    }
}