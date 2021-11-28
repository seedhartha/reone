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

#include "../graphics/texture.h"
#include "../graphics/types.h"

#include "types.h"

namespace reone {

namespace resource {

class GffStruct;
class Strings;

} // namespace resource

namespace graphics {

class Context;
class Font;
class Fonts;
class Meshes;
class Shaders;
class Texture;
class Textures;
class Window;

} // namespace graphics

namespace gui {

class GUI;

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

    static ControlType getType(const resource::GffStruct &gffs);
    static std::string getTag(const resource::GffStruct &gffs);
    static std::string getParent(const resource::GffStruct &gffs);

    virtual ~Control() = default;

    virtual void load(const resource::GffStruct &gffs);
    virtual void update(float dt) {}

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

    Border &border() const { return *_border; }
    const Extent &extent() const { return _extent; }
    const Border &hilight() const { return *_hilight; }
    const std::string &tag() const { return _tag; }
    const Text &text() const { return _text; }
    const std::vector<std::string> &textLines() const { return _textLines; }
    const std::string &sceneName() const { return _sceneName; }

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

    virtual void draw(const glm::ivec2 &offset, const std::vector<std::string> &text);

    // END Rendering

    // Event listeners

    void setOnClick(std::function<void()> fn) { _onClick = std::move(fn); }
    void setOnFocusChanged(std::function<void(bool)> fn) { _onFocusChanged = std::move(fn); }

    // END Event listeners

protected:
    GUI &_gui;
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

    graphics::Context &_context;
    graphics::Fonts &_fonts;
    graphics::Meshes &_meshes;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;
    graphics::Window &_window;
    resource::Strings &_strings;

    // END Services

    // Event listeners

    std::function<void()> _onClick;
    std::function<void(bool)> _onFocusChanged;

    // END Event listeners

    Control(
        GUI &gui,
        ControlType type,
        graphics::Context &context,
        graphics::Fonts &fonts,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Window &window,
        resource::Strings &strings) :
        _gui(gui),
        _type(type),
        _context(context),
        _fonts(fonts),
        _meshes(meshes),
        _shaders(shaders),
        _textures(textures),
        _window(window),
        _strings(strings) {
    }

    void drawBorder(const Border &border, const glm::ivec2 &offset, const glm::ivec2 &size);
    void drawText(const std::vector<std::string> &lines, const glm::ivec2 &offset, const glm::ivec2 &size);

    virtual const glm::vec3 &getBorderColor() const;

private:
    void loadExtent(const resource::GffStruct &gffs);
    void loadBorder(const resource::GffStruct &gffs);
    void loadText(const resource::GffStruct &gffs);
    void loadHilight(const resource::GffStruct &gffs);

    void updateTransform();
    void updateTextLines();

    void getTextPosition(glm::ivec2 &position, int lineCount, const glm::ivec2 &size, graphics::TextGravity &gravity) const;
};

} // namespace gui

} // namespace reone
