/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../../gui/gui.h"
#include "../../../scene/modelscenenode.h"
#include "../../../scene/scenegraph.h"

#include "../../object/creature.h"

#include "classselect.h"
#include "nameentry.h"
#include "portraitselect.h"
#include "quick.h"
#include "quickorcustom.h"

namespace reone {

namespace game {

enum class CharGenScreen {
    ClassSelection,
    QuickOrCustom,
    Quick,
    PortraitSelection,
    Name
};

class Game;

class CharacterGeneration : public gui::GUI {
public:
    CharacterGeneration(Game *game);

    void load() override;
    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void render() const override;
    void render3D() const override;

    void finish();
    void cancel();
    void openClassSelection();
    void openNameEntry();
    void openPortraitSelection();
    void openQuick();
    void openQuickOrCustom();

    const CreatureConfiguration &character() const;

    void setCharacter(const CreatureConfiguration &config);
    void setQuickStep(int step);

private:
    Game *_game { nullptr };
    CharGenScreen _screen { CharGenScreen::ClassSelection };
    CreatureConfiguration _character;
    std::unique_ptr<Creature> _creature;

    // Sub GUI

    std::unique_ptr<ClassSelection> _classSelection;
    std::unique_ptr<QuickOrCustom> _quickOrCustom;
    std::unique_ptr<QuickCharacterGeneration> _quick;
    std::unique_ptr<PortraitSelection> _portraitSelection;
    std::unique_ptr<NameEntry> _nameEntry;

    // END Sub GUI

    void loadCharacterModel();
    void updateAttributes();
    void changeScreen(CharGenScreen screen);

    gui::GUI *getSubGUI() const;
    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(scene::SceneGraph &sceneGraph);

    // Loading

    void loadClassSelection();
    void loadQuickOrCustom();
    void loadQuick();
    void loadPortraitSelection();
    void loadNameEntry();

    // END Loading
};

} // namespace game

} // namespace reone
