#pragma once

#include <memory>

#include "texture.h"

namespace reone {

namespace render {

enum class TextAlign {
    Left,
    Center,
    Right
};

class Font {
public:
    Font() = default;

    void load(const std::shared_ptr<Texture> &texture);
    void initGL();

    void render(
        const std::string &text,
        const glm::mat4 &transform,
        const glm::vec3 &color = glm::vec3(1.0f, 1.0f, 1.0f),
        TextAlign align = TextAlign::Center) const;

    float measure(const std::string &text) const;
    float height() const;

private:
    bool _glInited { false };
    std::vector<float> _vertices;
    std::vector<uint16_t> _indices;
    int _glyphCount { 0 };
    float _height { 0.0f };
    std::vector<float> _glyphWidths;
    std::shared_ptr<Texture> _texture;
    uint32_t _vertexBufferId { 0 };
    uint32_t _indexBufferId { 0 };
    uint32_t _vertexArrayId { 0 };
};

} // namespace render

} // namespace reone
