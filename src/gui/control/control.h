/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include <cstdint>
#include <functional>
#include <memory>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "../../render/font.h"
#include "../../render/framebuffer.h"
#include "../../render/texture.h"
#include "../../resource/gfffile.h"
#include "../../scene/node/modelscenenode.h"
#include "../../scene/pipeline/control.h"
#include "../../scene/scenegraph.h"
#include "../types.h"

namespace reone {

namespace gui {

class GUI;

/**
 * GUI control. Can render itself and handle events.
 */
class Control {
public:
    enum class TextAlign {
        LeftCenter = 9,
        CenterBottom = 10,
        CenterCenter = 18,

        CenterTop = 0x1000
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
        std::shared_ptr<scene::ModelSceneNode> model;
        std::unique_ptr<scene::SceneGraph> sceneGraph;
    };

    static ControlType getType(const resource::GffStruct &gffs);
    static std::string getTag(const resource::GffStruct &gffs);
    static std::string getParent(const resource::GffStruct &gffs);
    static std::unique_ptr<Control> of(GUI *gui, ControlType type, const std::string &tag);

    virtual ~Control() = default;

    virtual void load(const resource::GffStruct &gffs);
    virtual void update(float dt);
    virtual void stretch(float x, float y);

    bool isClickable() const;
    bool isDisabled() const;
    bool isFocusable() const;
    bool isVisible() const;

    Border &border() const;
    const Extent &extent() const;
    const Border &hilight() const;
    const std::string &tag() const;
    const Text &text() const;

    void setBorder(const Border &border);
    void setBorderFill(const std::string &resRef);
    void setBorderFill(const std::shared_ptr<render::Texture> &texture);
    void setBorderColor(const glm::vec3 &color);
    void setBorderColorOverride(const glm::vec3 &color);
    void setDisabled(bool disabled);
    void setDiscardColor(const glm::vec3 &color);
    virtual void setExtent(const Extent &extent);
    virtual void setFocus(bool focus);
    void setFocusable(bool focusable);
    void setHilight(const Border &hilight);
    void setHilightColor(const glm::vec3 &color);
    void setPadding(int padding);
    void setScene3D(std::unique_ptr<Scene3D> scene);
    void setText(const Text &text);
    void setTextColor(const glm::vec3 & color);
    void setTextMessage(const std::string &text);
    void setUseBorderColorOverride(bool use);
    void setVisible(bool visible);

    // User input

    virtual bool handleMouseMotion(int x, int y);
    virtual bool handleMouseWheel(int x, int y);
    virtual bool handleClick(int x, int y);

    // END User input

    // Rendering

    virtual void render(const glm::ivec2 &offset, const std::string &textOverride = "") const;
    void render3D(const glm::ivec2 &offset) const;

    // END Rendering

protected:
    GUI *_gui { nullptr };
    ControlType _type { ControlType::Invalid };
    int _id { -1 };
    std::string _tag;
    Extent _extent;
    std::shared_ptr<Border> _border;
    std::shared_ptr<Border> _hilight;
    Text _text;
    std::unique_ptr<Scene3D> _scene3d;
    int _padding { 0 };
    glm::mat4 _transform { 1.0f };
    bool _visible { true };
    bool _disabled { false };
    bool _focus { false };
    bool _focusable { true };
    bool _clickable { false };
    bool _discardEnabled { false };
    glm::vec3 _discardColor { false };
    glm::vec3 _borderColorOverride { 1.0f };
    bool _useBorderColorOverride { false };

    Control(GUI *, ControlType type);

    void drawBorder(const Border &border, const glm::ivec2 &offset, const glm::ivec2 &size) const;
    void drawText(const std::string &text, const glm::ivec2 &offset, const glm::ivec2 &size) const;

    virtual const glm::vec3 &getBorderColor() const;

private:
    std::unique_ptr<scene::ControlRenderPipeline> _pipeline;

    Control(const Control &) = delete;
    Control &operator=(const Control &) = delete;

    void updateTransform();
    void loadExtent(const resource::GffStruct &gffs);
    void loadBorder(const resource::GffStruct &gffs);
    void loadText(const resource::GffStruct &gffs);
    void loadHilight(const resource::GffStruct &gffs);
    std::vector<std::string> breakText(const std::string &text, int maxWidth) const;
    void getTextPosition(glm::ivec2 &position, int lineCount, const glm::ivec2 &size) const;
};

} // namespace gui

} // namespace reone
