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

#include "reone/game/gui/console.h"

#include "reone/game/debug.h"
#include "reone/game/di/services.h"
#include "reone/game/effect/damage.h"
#include "reone/game/game.h"
#include "reone/game/location.h"
#include "reone/game/object/creature.h"
#include "reone/game/party.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/font.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/resource/provider/fonts.h"
#include "reone/resource/resources.h"
#include "reone/scene/types.h"
#include "reone/script/executioncontext.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"
#include "reone/script/variable.h"
#include "reone/system/logutil.h"

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static constexpr int kMaxOutputLineCount = 100;
static constexpr int kVisibleLineCount = 15;

static constexpr float kTextOffset = 3.0f;

void Console::init() {
    _font = _services.resource.fonts.get("fnt_console");

    addCommand("clear", "c", "clear console", std::bind(&Console::cmdClear, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("info", "i", "information on selected object", std::bind(&Console::cmdInfo, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("listglobals", "lg", "list global variables", std::bind(&Console::cmdListGlobals, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("listlocals", "ll", "list local variables", std::bind(&Console::cmdListLocals, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("runscript", "rs", "run script", std::bind(&Console::cmdRunScript, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("listanim", "la", "list animations of selected object", std::bind(&Console::cmdListAnim, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("playanim", "pa", "play animation on selected object", std::bind(&Console::cmdPlayAnim, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("warp", "w", "warp to a module", std::bind(&Console::cmdWarp, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("kill", "k", "kill selected object", std::bind(&Console::cmdKill, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("additem", "ai", "add item to selected object", std::bind(&Console::cmdAddItem, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("givexp", "xp", "give experience to selected creature", std::bind(&Console::cmdGiveXP, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("showwalkmesh", "sw", "toggle rendering walkmesh", std::bind(&Console::cmdShowWalkmesh, this, std::placeholders::_1, std::placeholders::_2));
    addCommand("showtriggers", "st", "toggle rendering triggers", std::bind(&Console::cmdShowTriggers, this, std::placeholders::_1, std::placeholders::_2));

    addCommand("help", "h", "list console commands", std::bind(&Console::cmdHelp, this, std::placeholders::_1, std::placeholders::_2));
}

void Console::addCommand(std::string name, std::string alias, std::string description, CommandHandler handler) {
    Command cmd;
    cmd.name = std::move(name);
    cmd.alias = std::move(alias);
    cmd.description = std::move(description);
    cmd.handler = std::move(handler);

    _commands.push_back(cmd);
    _commandByName.insert(std::make_pair(cmd.name, cmd));

    if (!cmd.alias.empty()) {
        _commandByAlias.insert(std::make_pair(cmd.alias, cmd));
    }
}

bool Console::handle(const SDL_Event &event) {
    if (_open && _input.handle(event)) {
        return true;
    }
    switch (event.type) {
    case SDL_MOUSEWHEEL:
        return handleMouseWheel(event.wheel);
    case SDL_KEYUP:
        return handleKeyUp(event.key);
    default:
        return false;
    }
}

bool Console::handleMouseWheel(const SDL_MouseWheelEvent &event) {
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

bool Console::handleKeyUp(const SDL_KeyboardEvent &event) {
    if (_open) {
        switch (event.keysym.sym) {
        case SDLK_BACKQUOTE:
            _open = false;
            return true;

        case SDLK_RETURN: {
            std::string text(_input.text());
            if (!text.empty()) {
                executeInputText();
                _history.push(_input.text());
                _input.clear();
            }
            return true;
        }
        case SDLK_UP:
            if (!_history.empty()) {
                _input.setText(_history.top());
                _history.pop();
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
    std::vector<std::string> tokens;
    boost::split(tokens, _input.text(), boost::is_space(), boost::token_compress_on);
    if (tokens.empty()) {
        return;
    }
    CommandHandler handler;
    auto maybeCmdByName = _commandByName.find(tokens[0]);
    if (maybeCmdByName != _commandByName.end()) {
        handler = maybeCmdByName->second.handler;
    } else {
        auto maybeCmdByAlias = _commandByAlias.find(tokens[0]);
        if (maybeCmdByAlias != _commandByAlias.end()) {
            handler = maybeCmdByAlias->second.handler;
        }
    }
    if (handler) {
        try {
            handler(_input.text(), std::move(tokens));
        } catch (const std::invalid_argument &) {
            print("Invalid argument");
        }
    } else {
        print("Unknown command");
    }
}

void Console::draw() {
    _services.graphics.context.withBlending(BlendMode::Normal, [this]() {
        drawBackground();
        drawLines();
    });
}

void Console::drawBackground() {
    float height = kVisibleLineCount * _font->height();

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(_game.options().graphics.width, height, 1.0f));

    _services.graphics.uniforms.setGeneral([this, transform](auto &general) {
        general.resetLocals();
        general.projection = _services.graphics.window.getOrthoProjection();
        general.model = std::move(transform);
        general.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        general.alpha = 0.5f;
    });
    _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::simpleColor));
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

void Console::drawLines() {
    float height = kVisibleLineCount * _font->height();

    glm::vec3 position(kTextOffset, height - 0.5f * _font->height(), 0.0f);

    // Input

    std::string text("> " + _input.text());
    _font->draw(text, position, glm::vec3(1.0f), TextGravity::RightCenter);

    // Output

    for (int i = 0; i < kVisibleLineCount - 1 && i < static_cast<int>(_output.size()) - _outputOffset; ++i) {
        const std::string &line = _output[static_cast<size_t>(i) + _outputOffset];
        position.y -= _font->height();
        _font->draw(line, position, glm::vec3(1.0f), TextGravity::RightCenter);
    }
}

void Console::cmdClear(std::string input, std::vector<std::string> tokens) {
    _output.clear();
    _outputOffset = 0;
}

void Console::cmdInfo(std::string input, std::vector<std::string> tokens) {
    auto object = _game.module()->area()->selectedObject();
    if (!object) {
        print("No object is selected");
        return;
    }
    glm::vec3 position(object->position());

    std::stringstream ss;
    ss << std::setprecision(2) << std::fixed
       << "id=" << object->id()
       << " "
       << "tag=\"" << object->tag() << "\""
       << " "
       << "tpl=\"" << object->blueprintResRef() << "\""
       << " "
       << "pos=[" << position.x << ", " << position.y << ", " << position.z << "]";

    switch (object->type()) {
    case ObjectType::Creature: {
        auto creature = std::static_pointer_cast<Creature>(object);
        ss << " "
           << "app=" << creature->appearance()
           << " "
           << "fac=" << static_cast<int>(creature->faction());
        break;
    }
    case ObjectType::Placeable: {
        auto placeable = std::static_pointer_cast<Placeable>(object);
        ss << " "
           << "app=" << placeable->appearance();
        break;
    }
    default:
        break;
    }

    print(ss.str());
}

void Console::cmdListGlobals(std::string input, std::vector<std::string> tokens) {
    auto &strings = _game.globalStrings();
    for (auto &var : strings) {
        print(var.first + " = " + var.second);
    }

    auto &booleans = _game.globalBooleans();
    for (auto &var : booleans) {
        print(var.first + " = " + (var.second ? "true" : "false"));
    }

    auto &numbers = _game.globalNumbers();
    for (auto &var : numbers) {
        print(var.first + " = " + std::to_string(var.second));
    }

    auto &locations = _game.globalLocations();
    for (auto &var : locations) {
        print(str(boost::format("%s = (%.04f, %.04f, %.04f, %.04f") %
                  var.first %
                  var.second->position().x %
                  var.second->position().y %
                  var.second->position().z %
                  var.second->facing()));
    }
}

void Console::cmdListLocals(std::string input, std::vector<std::string> tokens) {
    auto object = _game.module()->area()->selectedObject();
    if (!object) {
        print("No object is selected");
        return;
    }

    auto &booleans = object->localBooleans();
    for (auto &var : booleans) {
        print(std::to_string(var.first) + " -> " + (var.second ? "true" : "false"));
    }

    auto &numbers = object->localNumbers();
    for (auto &var : numbers) {
        print(std::to_string(var.first) + " -> " + std::to_string(var.second));
    }
}

void Console::cmdListAnim(std::string input, std::vector<std::string> tokens) {
    auto object = _game.module()->area()->selectedObject();
    if (!object) {
        object = _game.party().getLeader();
        if (!object) {
            print("No object is selected");
            return;
        }
    }

    std::string substr;
    if (static_cast<int>(tokens.size()) > 1) {
        substr = tokens[1];
    }

    auto model = std::static_pointer_cast<ModelSceneNode>(object->sceneNode());
    std::vector<std::string> anims(model->model().getAnimationNames());
    sort(anims.begin(), anims.end());

    for (auto &anim : anims) {
        if (substr.empty() || boost::contains(anim, substr)) {
            print(anim);
        }
    }
}

void Console::cmdPlayAnim(std::string input, std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        print("Usage: playanim anim_name");
        return;
    }
    auto object = _game.module()->area()->selectedObject();
    if (!object) {
        object = _game.party().getLeader();
        if (!object) {
            print("No object is selected");
            return;
        }
    }
    auto model = std::static_pointer_cast<ModelSceneNode>(object->sceneNode());
    model->playAnimation(tokens[1], nullptr, AnimationProperties::fromFlags(AnimationFlags::loop));
}

void Console::cmdKill(std::string input, std::vector<std::string> tokens) {
    auto object = _game.module()->area()->selectedObject();
    if (!object) {
        print("No object is selected");
        return;
    }
    auto effect = _game.newEffect<DamageEffect>(
        100000,
        DamageType::Universal,
        DamagePower::Normal,
        std::shared_ptr<Creature>());
    object->applyEffect(std::move(effect), DurationType::Instant);
}

void Console::cmdAddItem(std::string input, std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        print("Usage: additem item_tpl [size]");
        return;
    }
    auto object = _game.module()->area()->selectedObject();
    if (!object) {
        object = _game.party().getLeader();
        if (!object) {
            print("No object is selected");
            return;
        }
    }
    int stackSize = static_cast<int>(tokens.size()) > 2 ? stoi(tokens[2]) : 1;
    object->addItem(tokens[1], stackSize);
}

void Console::cmdGiveXP(std::string input, std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        print("Usage: givexp amount");
        return;
    }

    auto object = _game.module()->area()->selectedObject();
    if (!object) {
        object = _game.party().getLeader();
    }
    if (!object || object->type() != ObjectType::Creature) {
        print("No creature is selected");
        return;
    }

    int amount = stoi(tokens[1]);
    std::static_pointer_cast<Creature>(object)->giveXP(amount);
}

void Console::cmdWarp(std::string input, std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        print("Usage: warp module");
        return;
    }
    _game.loadModule(tokens[1]);
}

void Console::cmdRunScript(std::string input, std::vector<std::string> tokens) {
    if (tokens.size() < 3) {
        print("Usage: runscript resref caller_id [triggerrer_id [event_number [script_var]]], e.g. runscript k_ai_master 1 2 3 4");
        return;
    }

    std::string resRef = tokens[1];
    auto callerId = static_cast<uint32_t>(stoi(tokens[2]));
    auto triggerrerId = tokens.size() > 3 ? static_cast<uint32_t>(stoi(tokens[3])) : kObjectInvalid;
    int eventNumber = tokens.size() > 4 ? stoi(tokens[4]) : -1;
    int scriptVar = tokens.size() > 5 ? stoi(tokens[5]) : -1;

    int result = _game.scriptRunner().run(resRef, callerId, triggerrerId, eventNumber, scriptVar);
    print(str(boost::format("%s -> %d") % resRef % result));
}

void Console::cmdShowWalkmesh(std::string input, std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        print("Usage: showwalkmesh 1|0");
        return;
    }
    bool show = stoi(tokens[1]);
    setShowWalkmesh(show);
}

void Console::cmdShowTriggers(std::string input, std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        print("Usage: showtriggers 1|0");
        return;
    }
    bool show = stoi(tokens[1]);
    setShowTriggers(show);
}

void Console::cmdHelp(std::string input, std::vector<std::string> tokens) {
    for (auto &cmd : _commands) {
        auto text = cmd.name;
        if (!cmd.alias.empty()) {
            text += " (" + cmd.alias + ")";
        }
        text += ": " + cmd.description;
        print(text);
    }
}

void Console::print(const std::string &text) {
    float maxWidth = _game.options().graphics.width - 2.0f * kTextOffset;

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

} // namespace game

} // namespace reone
