/*
 * Copyright (c) 2020-2023 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "types.h"

namespace reone {

namespace graphics {

class Texture;

class Material : boost::noncopyable {
public:
    using TextureUnit = int;
    using TextureUnitToTexture = std::unordered_map<TextureUnit, std::reference_wrapper<Texture>>;

    Material(std::string programId) :
        _programId(std::move(programId)) {
    }

    const std::string &programId() const {
        return _programId;
    }

    const TextureUnitToTexture &textures() const {
        return _textures;
    }

    const glm::mat3x4 &uv() const {
        return _uv;
    }

    int heightMapFrame() const {
        return _heightMapFrame;
    }

    const glm::vec4 &color() const {
        return _color;
    }

    const glm::vec3 &ambient() const {
        return _ambient;
    }

    const glm::vec3 &diffuse() const {
        return _diffuse;
    }

    const glm::vec3 &selfIllumColor() const {
        return _selfIllumColor;
    }

    bool affectedByShadows() const {
        return _affectedByShadows;
    }

    bool affectedByFog() const {
        return _affectedByFog;
    }

    const std::optional<CullFaceMode> &cullFaceMode() const {
        return _cullFaceMode;
    }

    void setTexture(TextureUnit unit, Texture &texture) {
        _textures.insert_or_assign(unit, texture);
    }

    void setUV(glm::mat3x4 uv) {
        _uv = std::move(uv);
    }

    void setHeightMapFrame(int frame) {
        _heightMapFrame = frame;
    }

    void setColor(glm::vec4 color) {
        _color = std::move(color);
    }

    void setAmbient(glm::vec3 ambient) {
        _ambient = std::move(ambient);
    }

    void setDiffuse(glm::vec3 diffuse) {
        _diffuse = std::move(diffuse);
    }

    void setSelfIllumColor(glm::vec3 emission) {
        _selfIllumColor = std::move(emission);
    }

    void setAffectedByShadows(bool affected) {
        _affectedByShadows = affected;
    }

    void setAffectedByFog(bool affected) {
        _affectedByFog = affected;
    }

    void setCullFaceMode(CullFaceMode mode) {
        _cullFaceMode = mode;
    }

private:
    std::string _programId;

    TextureUnitToTexture _textures;
    glm::mat3x4 _uv {1.0f};
    int _heightMapFrame {0};

    glm::vec4 _color {0.0f};
    glm::vec3 _ambient {0.2f};
    glm::vec3 _diffuse {0.8f};
    glm::vec3 _selfIllumColor {0.0f};

    bool _affectedByShadows {false};
    bool _affectedByFog {false};

    std::optional<CullFaceMode> _cullFaceMode;
};

} // namespace graphics

} // namespace reone
