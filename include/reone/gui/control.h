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

#include "reone/graphics/texture.h"
#include "reone/graphics/types.h"

#include "schema/gui.h"
#include "types.h"

namespace reone {

namespace resource {

class Gff;
class IStrings;

} // namespace resource

namespace graphics {

class Font;
class Texture;

struct GraphicsServices;

} // namespace graphics

namespace scene {

class ISceneGraphs;

}

namespace gui {

class IGUI;

/**
 * GUI control. Can render itself and handle events.
 */
class Control : boost::noncopyable {
public:
    static constexpr int kStretchLeft = 1;
    static constexpr int kStretchTop = 2;
    static constexpr int kStretchWidth = 4;
    static constexpr int kStretchHeight = 8;
    static constexpr int kStretchAll = kStretchLeft | kStretchTop | kStretchWidth | kStretchHeight;

    enum class TextAlign {
        LeftTop = 9,
        CenterTop = 10,
        RightCenter = 12,
        LeftCenter = 17,
        CenterCenter = 18,
        RightCenter2 = 20,

        CenterBottom = 0x1000
    };

    struct Extent {
        int left {0};
        int top {0};
        int width {0};
        int height {0};

        Extent() = default;
        Extent(int left, int top, int width, int height);

        bool contains(int x, int y) const;
        void getCenter(int &x, int &y) const;
    };

    struct Border {
        std::shared_ptr<graphics::Texture> corner;
        std::shared_ptr<graphics::Texture> edge;
        std::shared_ptr<graphics::Texture> fill;
        glm::vec3 color {1.0f};
        int dimension {0};
    };

    struct Text {
        std::string text;
        std::shared_ptr<graphics::Font> font;
        glm::vec3 color {1.0f};
        TextAlign align {TextAlign::CenterCenter};
    };

    static ControlType getType(const schema::GUI_BASECONTROL &gui);
    static std::string getTag(const schema::GUI_BASECONTROL &gui);
    static std::string getParent(const schema::GUI_BASECONTROL &gui);

    virtual ~Control() = default;

    virtual void load(const schema::GUI_BASECONTROL &gui, bool protoItem = false);
    virtual void update(float dt);

    void updateTransform();
    void updateTextLines();

    /**
     * Stretches this control in both directions.
     *
     * @param mask bitmask, specifying which components to stretch
     */
    virtual void stretch(float x, float y, int mask = kStretchAll);

    bool isClickable() const { return _clickable; }
    bool isDisabled() const { return _disabled; }
    bool isFocusable() const { return _focusable; }
    bool isVisible() const { return _visible; }

    int id() const { return _id; }
    int padding() const { return _padding; }
    Border &border() const { return *_border; }
    const Extent &extent() const { return _extent; }
    const Border &hilight() const { return *_hilight; }
    const std::string &tag() const { return _tag; }
    const Text &text() const { return _text; }
    const std::vector<std::string> &textLines() const { return _textLines; }
    const std::string &sceneName() const { return _sceneName; }

    void setId(int id) { _id = id; }
    void setTag(std::string tag) { _tag = std::move(tag); }
    void setBorder(Border border);
    void setBorderFill(std::string resRef);
    void setBorderFill(std::shared_ptr<graphics::Texture> texture);
    void setBorderColor(glm::vec3 color);
    void setBorderColorOverride(glm::vec3 color);
    void setDisabled(bool disabled);
    void setDiscardColor(glm::vec3 color);
    virtual void setExtent(Extent extent);
    virtual void setExtentHeight(int height);
    void setExtentTop(int top);
    virtual void setFocus(bool focus);
    void setFocusable(bool focusable);
    void setHeight(int height);
    void setHilight(Border hilight);
    void setHilightColor(glm::vec3 color);
    void setHilightFill(std::string resRef);
    void setHilightFill(std::shared_ptr<graphics::Texture> texture);
    void setPadding(int padding);
    void setSceneName(std::string name);
    void setText(Text text);
    void setTextColor(glm::vec3 color);
    void setTextMessage(std::string text);
    void setTextFont(std::shared_ptr<graphics::Font> font);
    void setUseBorderColorOverride(bool use);
    void setVisible(bool visible);

    // User input

    virtual bool handleMouseMotion(int x, int y);
    virtual bool handleMouseWheel(int x, int y);
    virtual bool handleClick(int x, int y);

    // END User input

    // Rendering

    virtual void draw(const glm::ivec2 &screenSize, const glm::ivec2 &offset, const std::vector<std::string> &text);

    // END Rendering

    // Event listeners

    void setOnClick(std::function<void()> fn) { _onClick = std::move(fn); }
    void setOnFocusChanged(std::function<void(bool)> fn) { _onFocusChanged = std::move(fn); }

    // END Event listeners

protected:
    IGUI &_gui;
    ControlType _type;

    int _id {-1};
    std::string _tag;
    Extent _extent;
    std::shared_ptr<Border> _border;
    std::shared_ptr<Border> _hilight;
    Text _text;
    std::string _sceneName;
    int _padding {0};
    glm::mat4 _transform {1.0f};
    bool _visible {true};
    bool _disabled {false};
    bool _focus {false};
    bool _focusable {true};
    bool _clickable {false};
    bool _discardEnabled {false};
    glm::vec3 _discardColor {false};
    glm::vec3 _borderColorOverride {1.0f};
    bool _useBorderColorOverride {false};
    std::vector<std::string> _textLines;

    // Services

    graphics::GraphicsServices &_graphicsSvc;
    resource::IStrings &_strings;
    scene::ISceneGraphs &_sceneGraphs;

    // END Services

    // Event listeners

    std::function<void()> _onClick;
    std::function<void(bool)> _onFocusChanged;

    // END Event listeners

    Control(
        IGUI &gui,
        ControlType type,
        scene::ISceneGraphs &sceneGraphs,
        graphics::GraphicsServices &graphicsSvc,
        resource::IStrings &strings) :
        _gui(gui),
        _type(type),
        _sceneGraphs(sceneGraphs),
        _graphicsSvc(graphicsSvc),
        _strings(strings) {
    }

    void drawBorder(const Border &border, const glm::ivec2 &offset, const glm::ivec2 &size);
    void drawText(const std::vector<std::string> &lines, const glm::ivec2 &offset, const glm::ivec2 &size);

    virtual const glm::vec3 &getBorderColor() const;

private:
    void loadExtent(const schema::GUI_EXTENT &gui);
    void loadBorder(const schema::GUI_BORDER &gui);
    void loadText(const schema::GUI_TEXT &gui);
    void loadHilight(const schema::GUI_BORDER &gui);

    void getTextPosition(glm::ivec2 &position, int lineCount, const glm::ivec2 &size, graphics::TextGravity &gravity) const;
};

} // namespace gui

} // namespace reone
