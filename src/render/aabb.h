#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace reone {

namespace render {

class AABB {
public:
    AABB() = default;
    AABB(const glm::vec3 &min, const glm::vec3 &max);

    AABB operator*(const glm::mat4 &m) const;

    void reset();
    void expand(const glm::vec3 &p);
    void expand(const AABB &aabb);

    bool contains(const glm::vec2 &point) const;
    bool contains(const glm::vec3 &point) const;
    bool intersectLine(const glm::vec3 &from, const glm::vec3 &to, float &distance) const;

    glm::vec3 size() const;

    const glm::vec3 &min() const;
    const glm::vec3 &max() const;
    const glm::vec3 &center() const;
    const glm::mat4 &transform() const;

private:
    bool _empty { true };
    glm::vec3 _min { 0.0f };
    glm::vec3 _max { 0.0f };
    glm::vec3 _center { 0.0f };
    glm::mat4 _transform { 1.0f };

    void updateTransform();
};

} // namespace render

} // namespace reone
