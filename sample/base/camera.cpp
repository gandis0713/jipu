#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vkt
{

void Camera::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
    m_eye = eye;
    m_center = center;
    m_up = up;

    m_view = glm::lookAt(eye, center, up);
}

glm::mat4 Camera::getViewMat() const
{
    return m_view;
}

glm::mat4 Camera::getProjectionMat() const
{
    return m_projection;
}

glm::vec3 Camera::getPosition() const
{
    return m_eye;
}

glm::vec3 Camera::getUp() const
{
    return m_up;
}

Camera::Type Camera::getType() const
{
    return m_type;
}

OrthographicCamera::OrthographicCamera(float left, float right,
                                       float bottom, float top,
                                       float near, float far)
{
    m_projection = glm::ortho(left, right, bottom, top, near, far);

    m_type = Type::kOrthographic;
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect,
                                     float near, float far)
{
    m_projection = glm::perspective(fov, aspect, near, far);

    m_type = Type::kPerspective;
}

} // namespace vkt