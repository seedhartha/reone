/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/common/exception/validation.h"
#include "reone/common/exception/validation.h"
#include "../../graphics/fonts.h"
#include "../../graphics/options.h"
#include "../../graphics/services.h"
#include "../../gui/control/label.h"
#include "../../gui/control/listbox.h"

#include "../action/movetoobject.h"
#include "../game.h"

using namespace std;

using namespace reone::gui;

namespace reone {

namespace game {

static constexpr int kNumLines = 20;

static const string kFontResRef = "fnt_console";

void Console::init() {
    auto font = _graphicsSvc.fonts.get(kFontResRef);

    // Lines list box

    auto lbLinesProtoItem = newLabel(ListBox::itemControlId(0, -1));
    lbLinesProtoItem->setExtent(glm::ivec4(0, 0, _graphicsOpt.width, font->height()));
    lbLinesProtoItem->setFont(kFontResRef);

    auto lbLines = static_pointer_cast<ListBox>(newListBox(0));
    lbLines->setExtent(glm::ivec4(0, 0, _graphicsOpt.width, static_cast<int>(kNumLines * font->height())));
    lbLines->setProtoItem(lbLinesProtoItem.get());
    lbLines->initItemSlots();

    _lbLines = lbLines.get();

    // Input label

    auto lblInput = static_pointer_cast<Label>(newLabel(1));
    lblInput->setExtent(glm::ivec4(0, lbLines->extent()[1] + lbLines->extent()[3], _graphicsOpt.width, font->height()));

    auto inputText = make_unique<Control::Text>();
    inputText->font = kFontResRef;
    inputText->text = "> ";
    lblInput->setText(move(inputText));

    _lblInput = lblInput.get();

    // Root control

    auto rootControl = newLabel(-1);
    rootControl->setExtent(glm::ivec4(0, 0, _graphicsOpt.width, static_cast<int>((kNumLines + 1) * font->height())));
    rootControl->setBorderFill("black");
    rootControl->setAlpha(0.5f);
    rootControl->append(*lbLines);
    rootControl->append(*lblInput);

    _rootControl = rootControl.get();
}

bool Console::handle(const SDL_Event &e) {
    if (_textInput.handle(e)) {
        _lblInput->setText("> " + _textInput.text());
        return true;
    }
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
        auto command = _textInput.text();
        if (!command.empty()) {
            _textInput.clear();
            _lblInput->setText("> ");
            onEnterCommand(command);
        }
        return true;
    }
    return false;
}

void Console::onEnterCommand(const string &command) {
    vector<string> tokens;
    boost::split(tokens, command, boost::is_space(), boost::token_compress_on);

    if (tokens[0] == "clear") {
        _lbLines->clearItems();
    } else {
        auto arguments = vector<string>(tokens.begin() + 1, tokens.end());
        try {
            if (tokens[0] == "actionmovetoobject") {
                executeActionMoveToObject(move(arguments));
            }
        } catch (const ValidationException &ex) {
            _lbLines->appendItem(ListBox::Item {tokens[0] + ": " + string(ex.what())}, true);
        }
    }
}

void Console::executeActionMoveToObject(vector<string> arguments) {
    if (arguments.size() < 2ll) {
        throw ValidationException("Command requires at least 2 arguments");
    }
    auto subjectTag = arguments[0];
    auto subject = _game.objectByTag(subjectTag);
    if (!subject) {
        throw ValidationException("Invalid subject: " + subjectTag);
    }

    auto moveToTag = arguments[1];
    auto moveTo = _game.objectByTag(moveToTag);
    if (!moveTo) {
        throw ValidationException("Invalid moveTo: " + moveToTag);
    }

    bool run = false;
    if (arguments.size() >= 3) {
        run = stoi(arguments[2]) != 0;
    }

    float range = 1.0f;
    if (arguments.size() >= 4) {
        range = stof(arguments[3]);
    }

    auto action = make_shared<MoveToObjectAction>(*moveTo, run, range);
    subject->clearAllActions();
    subject->enqueue(move(action));
}

} // namespace game

} // namespace reone
