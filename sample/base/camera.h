#pragma once

#include <glm/glm.hpp>

namespace vkt
{

class Camera
{
public:
    enum class Type : uint8_t
    {
        kUndefined = 0,
        kPerspective,
        kOrthographic,
    };

    void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

    glm::mat4 getViewMat() const;
    glm::mat4 getProjectionMat() const;

    glm::vec3 getPosition() const;
    glm::vec3 getUp() const;

    Type getType() const;

protected:
    alignas(1) Type m_type = Type::kUndefined;
    alignas(16) glm::mat4 m_projection;
    alignas(16) glm::mat4 m_view;

    alignas(16) glm::vec3 m_eye;    // position
    alignas(16) glm::vec3 m_center; // target
    alignas(16) glm::vec3 m_up;

    // alignas(4) float m_aspect = 1.0f;
    alignas(4) float m_near = 0.1f;
    alignas(4) float m_far = 1000.0f;
};

class OrthographicCamera : public Camera
{
public:
    explicit OrthographicCamera(float left, float right,
                                float bottom, float top,
                                float near, float far);

private:
    alignas(4) float m_left = -1.0f;
    alignas(4) float m_right = 1.0f;
    alignas(4) float m_bottom = -1.0f;
    alignas(4) float m_top = 1.0f;
};

class PerspectiveCamera : public Camera
{
public:
    explicit PerspectiveCamera(float fov, float aspect,
                               float near, float far);

private:
    alignas(8) double m_fov = 45.0f;
};

} // namespace vkt