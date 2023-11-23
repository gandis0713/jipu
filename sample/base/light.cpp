#include "light.h"

namespace jipu
{

Light::Light(glm::vec3 position, glm::vec3 color)
    : m_position(position)
    , m_color(color)
{
}

void Light::setPosition(glm::vec3 position)
{
    m_position = position;
}

glm::vec3 Light::getPosition() const
{
    return m_position;
}

void Light::setColor(glm::vec3 color)
{
    m_color = color;
}

glm::vec3 Light::getColor() const
{
    return m_color;
}

} // namespace jipu