#pragma once

#include <glm/glm.hpp>

namespace vkt
{

class Light
{

public:
    explicit Light(glm::vec3 position, glm::vec3 color);

    void setPosition(glm::vec3 position);
    glm::vec3 getPosition() const;

    void setColor(glm::vec3 color);
    glm::vec3 getColor() const;

protected:
    glm::vec3 m_position;
    glm::vec3 m_color;
};

} // namespace vkt