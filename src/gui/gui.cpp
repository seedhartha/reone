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

#include "gui.h"

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../core/log.h"
#include "../resources/resources.h"
#include "../render/mesh/guiquad.h"
#include "../render/shaders.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace gui {

GUI::GUI(const GraphicsOptions &opts) : _gfxOpts(opts) {
    _aspect = _gfxOpts.width / static_cast<float>(_gfxOpts.height);
    _screenCenter.x = _gfxOpts.width / 2;
    _screenCenter.y = _gfxOpts.height / 2;
}

void GUI::load(const string &resRef, BackgroundType background) {
    info("GUI: load " + resRef);

    shared_ptr<GffStruct> gui(Resources.findGFF(resRef, ResourceType::Gui));
    assert(gui);

    loadBackground(background);

    _rootControl = Control::makeControl(*gui);

    switch (_scaling) {
        case ScalingMode::Center:
            _rootOffset.x = _screenCenter.x - _resolutionX / 2;
            _rootOffset.y = _screenCenter.y - _resolutionY / 2;
            break;
        case ScalingMode::Stretch:
            stretchControl(*_rootControl);
            break;
        default:
            break;
    }

    const Control::Extent &rootExtent = _rootControl->extent();
    _controlOffset = _rootOffset + glm::ivec2(rootExtent.left, rootExtent.top);

    for (auto &ctrlGffs : gui->getList("CONTROLS")) {
        unique_ptr<Control> control(Control::makeControl(ctrlGffs));
        if (!control) continue;

        switch (_scaling) {
            case ScalingMode::PositionRelativeToCenter:
                positionRelativeToCenter(*control);
                break;
            case ScalingMode::Stretch:
                stretchControl(*control);
                break;
            default:
                break;
        }
        control->setOnClick(bind(&GUI::onClick, this, _1));
        _controls.push_back(move(control));
    }
}

void GUI::positionRelativeToCenter(Control &control) {
    Control::Extent extent(control.extent());
    if (extent.left >= 0.5f * _resolutionX) {
        extent.left = extent.left - _resolutionX + _gfxOpts.width;
    }
    if (extent.top >= 0.5f * _resolutionY) {
        extent.top = extent.top - _resolutionY + _gfxOpts.height;
    }
    control.setExtent(move(extent));
}

void GUI::stretchControl(Control &control) {
    float aspectX = _gfxOpts.width / static_cast<float>(_resolutionX);
    float aspectY = _gfxOpts.height / static_cast<float>(_resolutionY);
    control.stretch(aspectX, aspectY);
}

void GUI::loadBackground(BackgroundType type) {
    string resRef;

    if ((_gfxOpts.width == 1600 && _gfxOpts.height == 1200) ||
        (_gfxOpts.width == 1280 && _gfxOpts.height == 960) ||
        (_gfxOpts.width == 1024 && _gfxOpts.height == 768) ||
        (_gfxOpts.width == 800 && _gfxOpts.height == 600)) {

        resRef = str(boost::format("%dx%d") % _gfxOpts.width % _gfxOpts.height);
    } else {
        resRef = "1600x1200";
    }

    switch (type) {
        case BackgroundType::Menu:
            resRef += "back";
            break;
        default:
            return;
    }

    _background = ResourceManager::instance().findTexture(resRef, TextureType::Diffuse);
    assert(_background);
}

bool GUI::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            return handleKeyDown(event.key.keysym.scancode);

        case SDL_KEYUP:
            return handleKeyUp(event.key.keysym.scancode);

        case SDL_MOUSEMOTION: {
            glm::ivec2 ctrlCoords(event.motion.x - _controlOffset.x, event.motion.y - _controlOffset.y);
            updateFocus(ctrlCoords.x, ctrlCoords.y);
            if (_focus) {
                _focus->handleMouseMotion(ctrlCoords.x, ctrlCoords.y);
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
            if (_focus && event.button.button == SDL_BUTTON_LEFT) {
                debug("GUI: click: " + _focus->tag());
                glm::ivec2 ctrlCoords(event.button.x - _controlOffset.x, event.button.y - _controlOffset.y);
                return _focus->handleClick(ctrlCoords.x, ctrlCoords.y);
            }
            break;

        case SDL_MOUSEWHEEL:
            if (_focus && _focus->handleMouseWheel(event.wheel.x, event.wheel.y)) return true;
            break;
    }

    return false;
}

bool GUI::handleKeyDown(SDL_Scancode key) {
    return false;
}

bool GUI::handleKeyUp(SDL_Scancode key) {
    return false;
}

void GUI::updateFocus(int x, int y) {
    resetFocus();

    for (auto it = _controls.rbegin(); it != _controls.rend(); ++it) {
        shared_ptr<Control> control(*it);
        if (!control->visible()) continue;

        const Control::Extent &extent = control->extent();
        if (extent.contains(x, y)) {
            _focus = control;
            _focus->setFocus(true);
            onFocusChanged(_focus->tag(), true);
            return;
        }
    }
}

void GUI::onFocusChanged(const string &control, bool focus) {
}

void GUI::update(float dt) {
    for (auto &control : _controls) {
        control->update(dt);
    }
}

void GUI::render() const {
    if (_background) drawBackground();
    if (_rootControl) _rootControl->render(_rootOffset);

    for (auto &control : _controls) {
        control->render(_controlOffset);
    }
}

void GUI::drawBackground() const {
    glm::mat4 transform(glm::scale(glm::mat4(1.0f), glm::vec3(_gfxOpts.width, _gfxOpts.height, 1.0f)));

    ShaderManager &shaders = Shaders;
    shaders.activate(ShaderProgram::BasicDiffuse);
    shaders.setUniform("model", transform);
    shaders.setUniform("color", glm::vec3(1.0f));
    shaders.setUniform("alpha", 1.0f);

    glActiveTexture(0);
    _background->bind();

    DefaultGuiQuad.render(GL_TRIANGLES);

    _background->unbind();
}

void GUI::render3D() const {
    for (auto &control : _controls) {
        control->render3D(_controlOffset);
    }
}

void GUI::resetFocus() {
    if (_focus) {
        _focus->setFocus(false);
        onFocusChanged(_focus->tag(), false);
        _focus = nullptr;
    }
}

void GUI::showControl(const string &tag) {
    for (auto &control : _controls) {
        if (control->tag() == tag) {
            control->setVisible(true);
            return;
        }
    }
}

void GUI::hideControl(const string &tag) {
    for (auto &control : _controls) {
        if (control->tag() == tag) {
            control->setVisible(false);
            return;
        }
    }
}

Control &GUI::getControl(const string &tag) const {
    auto it = find_if(
        _controls.begin(),
        _controls.end(),
        [&tag](const shared_ptr<Control> &ctrl) { return ctrl->tag() == tag; });

    return **it;
}

void GUI::onClick(const string &control) {
}

void GUI::onItemClicked(const string &control, const string &item) {
}

} // namespace gui

} // namespace reone
