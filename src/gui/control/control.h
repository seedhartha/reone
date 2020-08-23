/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <functional>
#include <memory>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "../../render/font.h"
#include "../../render/modelinstance.h"
#include "../../render/texture.h"
#include "../../resources/gfffile.h"
#include "../types.h"

namespace reone {

namespace gui {

/**
 * GUI control. Can render itself and handle events.
 */
class Control {
public:
    enum class TextAlign {
        LeftCenter = 9,
        CenterBottom = 10,
        CenterCenter = 18
    };

    struct Extent {
        int left { 0 };
        int top { 0 };
        int width { 0 };
        int height { 0 };

        Extent() = default;
        Extent(int left, int top, int width, int height);

        bool contains(int x, int y) const;
        void getCenter(int &x, int &y) const;
    };

    struct Border {
        std::shared_ptr<render::Texture> corner;
        std::shared_ptr<render::Texture> edge;
        std::shared_ptr<render::Texture> fill;
        glm::vec3 color { 1.0f };
        int dimension { 0 };
    };

    struct Text {
        std::string text;
        std::shared_ptr<render::Font> font;
        glm::vec3 color { 1.0f };
        TextAlign align { TextAlign::CenterCenter };
    };

    struct Scene3D {
        std::shared_ptr<render::ModelInstance> model;
        glm::mat4 transform { 1.0f };
    };

    static std::unique_ptr<Control> makeControl(const resources::GffStruct &gffs);

    virtual void load(const resources::GffStruct &gffs);

    // Event handling
    virtual bool handleMouseMotion(int x, int y);
    virtual bool handleMouseWheel(int x, int y);
    virtual bool handleClick(int x, int y);

    // Rendering
    virtual void initGL();
    virtual void render(const glm::ivec2 &offset, const std::string &textOverride = "") const;
    virtual void render3D(const glm::ivec2 &offset) const;

    virtual void update(float dt);
    virtual void stretch(float x, float y);

    void setVisible(bool visible);
    virtual void setFocus(bool focus);
    virtual void setExtent(const Extent &extent);
    void setBorder(const Border &border);
    void setHilight(const Border &hilight);
    void setText(const Text &text);
    void setTextMessage(const std::string &text);
    void setScene3D(const Scene3D &scene);

    const std::string &tag() const;
    const Extent &extent() const;
    Border &border() const;
    const Border &hilight() const;
    const Text &text() const;
    bool visible() const;

    void setOnClick(const std::function<void(const std::string &)> &fn);
    void setOnItemClicked(const std::function<void(const std::string &, const std::string &)> &fn);

protected:
    ControlType _type { ControlType::Invalid };
    int _id { -1 };
    std::string _tag;
    Extent _extent;
    std::shared_ptr<Border> _border;
    std::shared_ptr<Border> _hilight;
    Text _text;
    Scene3D _scene3d;
    int _padding { 0 };
    glm::mat4 _transform { 1.0f };
    bool _visible { true };
    bool _focus { false };
    std::function<void(const std::string &)> _onClick;
    std::function<void(const std::string &, const std::string &)> _onItemClicked;

    Control(ControlType type);
    Control(ControlType type, const std::string &tag);

    void drawBorder(const Border &border, const glm::ivec2 &offset) const;
    void drawText(const std::string &text, const glm::ivec2 &offset) const;

private:
    Control(const Control &) = delete;
    Control &operator=(const Control &) = delete;

    void updateTransform();
    void loadExtent(const resources::GffStruct &gffs);
    void loadBorder(const resources::GffStruct &gffs);
    void loadText(const resources::GffStruct &gffs);
    void loadHilight(const resources::GffStruct &gffs);
    std::vector<std::string> breakText(const std::string &text, int maxWidth) const;
    void getTextPosition(glm::ivec2 &position, int lineCount) const;
};

} // namespace gui

} // namespace reone
