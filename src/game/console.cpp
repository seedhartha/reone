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

#include "console.h"

#include "glm/ext.hpp"

#include "SDL2/SDL_opengl.h"

#include "../core/log.h"
#include "../render/font.h"
#include "../render/mesh/quad.h"
#include "../render/shaders.h"
#include "../resources/resources.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace game {

static const int kLineCount = 10;

Console::Console(const GraphicsOptions &opts) : _opts(opts) {
}

void Console::load() {
    _font = Resources.findFont("fnt_d16x16");
    assert(_font);
}

bool Console::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            return handleKeyDown(event.key);
        case SDL_KEYUP:
            return handleKeyUp(event.key);
        default:
            return false;
    }
}

bool Console::handleKeyDown(const SDL_KeyboardEvent &event) {
    if (!_open) return false;

    bool shift = event.keysym.mod & KMOD_SHIFT;
    bool digit = event.keysym.sym >= SDLK_0 && event.keysym.sym <= SDLK_9;
    bool letter = event.keysym.sym >= SDLK_a && event.keysym.sym <= SDLK_z;
    bool underscore = event.keysym.sym == SDLK_MINUS && shift;

    if (digit || letter || underscore ||
        event.keysym.sym == SDLK_SPACE ||
        event.keysym.sym == SDLK_COMMA ||
        event.keysym.sym == SDLK_PERIOD) {

        if (underscore) {
            _inputText += SDLK_UNDERSCORE;
        } else if (letter && shift) {
            _inputText += toupper(event.keysym.sym);
        } else {
            _inputText += event.keysym.sym;
        }

        return true;

    } else if (event.keysym.sym == SDLK_BACKSPACE) {
        if (!_inputText.empty()) {
            _inputText.resize(_inputText.size() - 1);
        }
    }

    return false;
}

bool Console::handleKeyUp(const SDL_KeyboardEvent &event) {
    if (_open) {
        switch (event.keysym.sym) {
            case SDLK_BACKQUOTE:
                _open = false;
                return true;

            case SDLK_RETURN:
                if (!_inputText.empty()) {
                    executeInputText();
                    _inputText.clear();
                }
                return true;

            default:
                return false;
        }
    } else {
        switch (event.keysym.sym) {
            case SDLK_BACKQUOTE:
                _open = true;
                return true;

            default:
                return false;
        }
    }
}

void Console::executeInputText() {
    debug(boost::format("Console: execute \"%s\"") % _inputText);
}

void Console::render() const {
    float height = kLineCount * _font->height();
    {
        glm::mat4 transform(1.0f);
        transform = glm::scale(transform, glm::vec3(_opts.width, height, 1.0f));

        LocalUniforms locals;
        locals.model = move(transform);
        locals.color = glm::vec3(0.0f);
        locals.alpha = 0.5f;

        Shaders.activate(ShaderProgram::GUIGUI, locals);
    }
    DefaultQuad.render(GL_TRIANGLES);

    string text("> " + _inputText);
    {
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(3.0f, height - 0.5f * _font->height(), 0.0f));

        _font->render(text, transform, glm::vec3(1.0f), TextGravity::Right);
    }
}

bool Console::isOpen() const {
    return _open;
}

} // namespace game

} // namespace reone
