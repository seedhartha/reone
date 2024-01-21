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

#include "console.h"

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/fonts.h"
#include "reone/system/checkutil.h"

using namespace reone::graphics;

namespace reone {

static constexpr int kMaxOutputLineCount = 100;
static constexpr int kVisibleLineCount = 15;

static constexpr float kTextOffset = 3.0f;

void Console::init() {
    checkThat(!_inited, "Must not be initialized");
    _font = _resourceSvc.fonts.get("fnt_console");

    registerCommand("clear", "", [this](const auto &tokens) {
        _output.clear();
        _outputOffset = 0;
    });
    registerCommand("help", "", [this](const auto &tokens) {
        for (auto &command : _commands) {
            if (!command.description.empty()) {
                printLine(str(boost::format("%s: %s") % command.name % command.description));
            } else {
                printLine(command.name);
            }
        }
    });

    _inited = true;
}

void Console::deinit() {
    if (!_inited) {
        return;
    }
    _font.reset();
    _inited = false;
}

void Console::registerCommand(std::string name, std::string description, CommandHandler handler) {
    Command command;
    command.name = std::move(name);
    command.description = std::move(description);
    command.handler = std::move(handler);
    _commands.push_back(std::move(command));

    auto &ref = _commands.back();
    _nameToCommand.insert({ref.name, ref});
}

void Console::printLine(const std::string &text) {
    float maxWidth = _graphicsOpt.width - 2.0f * kTextOffset;
    std::ostringstream ss;
    for (size_t i = 0; i < text.length(); ++i) {
        ss << text[i];
        std::string s = ss.str();
        float w = _font->measure(s);
        if (w >= maxWidth) {
            _output.push_front(s.substr(0, s.length() - 1));
            ss.str("");
            ss << text[i];
        }
    }
    if (ss.tellp() > 0) {
        _output.push_front(ss.str());
    }

    trimOutput();
    _outputOffset = 0;
}

void Console::trimOutput() {
    for (int i = static_cast<int>(_output.size()) - kMaxOutputLineCount; i > 0; --i) {
        _output.pop_back();
    }
}

bool Console::handle(const input::Event &event) {
    if (_open && _input.handle(event)) {
        return true;
    }
    switch (event.type) {
    case input::EventType::MouseWheel:
        return handleMouseWheel(event.wheel);
    case input::EventType::KeyUp:
        return handleKeyUp(event.key);
    default:
        return false;
    }
}

bool Console::handleMouseWheel(const input::MouseWheelEvent &event) {
    bool up = event.y < 0;
    if (up) {
        if (_outputOffset > 0) {
            --_outputOffset;
        }
    } else {
        if (_outputOffset < static_cast<int>(_output.size()) - kVisibleLineCount + 1) {
            ++_outputOffset;
        }
    }
    return true;
}

bool Console::handleKeyUp(const input::KeyEvent &event) {
    if (_open) {
        switch (event.code) {
        case input::KeyCode::Backquote:
            _open = false;
            return true;

        case input::KeyCode::Return: {
            std::string text(_input.text());
            if (!text.empty()) {
                executeInputText();
                _history.push(_input.text());
                _input.clear();
            }
            return true;
        }
        case input::KeyCode::Up:
            if (!_history.empty()) {
                _input.setText(_history.top());
                _history.pop();
            }
            return true;
        default:
            return false;
        }
    } else {
        switch (event.code) {
        case input::KeyCode::Backquote:
            _open = true;
            return true;

        default:
            return false;
        }
    }
}

void Console::executeInputText() {
    TokenList tokens;
    boost::split(tokens, _input.text(), boost::is_space(), boost::token_compress_on);
    if (tokens.empty()) {
        return;
    }
    auto commandByName = _nameToCommand.find(tokens[0]);
    if (commandByName == _nameToCommand.end()) {
        printLine("Unrecognized command: " + tokens[0]);
        return;
    }
    auto &handler = commandByName->second.get().handler;
    try {
        handler(tokens);
    } catch (const std::exception &ex) {
        printLine("Command failed: " + std::string(ex.what()));
    }
}

void Console::render() {
    if (!_open) {
        return;
    }
    _graphicsSvc.context.withBlendMode(BlendMode::Normal, [this]() {
        renderBackground();
        renderLines();
    });
}

void Console::renderBackground() {
    float height = kVisibleLineCount * _font->height();
    _graphicsSvc.uniforms.setGlobals([this](auto &globals) {
        globals.reset();
        globals.projection = glm::ortho(
            0.0f, static_cast<float>(_graphicsOpt.width),
            static_cast<float>(_graphicsOpt.height), 0.0f,
            0.0f, 100.0f);
    });
    auto transform = glm::scale(
        glm::translate(glm::vec3 {0.0f, _graphicsOpt.height - height, 0.0f}),
        glm::vec3 {_graphicsOpt.width, height, 1.0f});
    _graphicsSvc.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
        locals.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        locals.color.a = 0.5f;
    });
    _graphicsSvc.context.useProgram(_graphicsSvc.shaderRegistry.get(ShaderProgramId::mvpColor));
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
}

void Console::renderLines() {
    float height = kVisibleLineCount * _font->height();
    glm::vec3 position {kTextOffset, _graphicsOpt.height - 0.5f * _font->height(), 0.0f};

    // Input

    std::string text {"> " + _input.text()};
    _font->render(text, position, glm::vec3(1.0f), TextGravity::RightCenter);

    // Output

    for (int i = 0; i < kVisibleLineCount - 1 && i < static_cast<int>(_output.size()) - _outputOffset; ++i) {
        const auto &line = _output[static_cast<size_t>(i) + _outputOffset];
        position.y -= _font->height();
        _font->render(line, position, glm::vec3(1.0f), TextGravity::RightCenter);
    }
}

} // namespace reone
