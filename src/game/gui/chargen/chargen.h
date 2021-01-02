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

#include "../../../gui/gui.h"
#include "../../../scene/node/modelscenenode.h"
#include "../../../scene/scenegraph.h"

#include "../../object/creature.h"

#include "abilities.h"
#include "classselect.h"
#include "custom.h"
#include "feats.h"
#include "nameentry.h"
#include "portraitselect.h"
#include "skills.h"
#include "quick.h"
#include "quickorcustom.h"

namespace reone {

namespace game {

enum class CharGenScreen {
    ClassSelection,
    QuickOrCustom,
    Quick,
    PortraitSelection,
    Name,
    Custom,
    Abilities,
    Skills,
    Feats
};

class Game;

class CharacterGeneration : public gui::GUI {
public:
    enum class Type {
        Quick,
        Custom
    };

    CharacterGeneration(Game *game);

    void load() override;
    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void render() const override;
    void render3D() const override;

    void startQuick();
    void startCustom();
    void finish();
    void cancel();

    void openClassSelection();
    void openNameEntry();
    void openAbilities();
    void openSkills();
    void openFeats();
    void openPortraitSelection();
    void openQuick();
    void openQuickOrCustom();
    void openCustom();
    void openSteps();

    void goToNextStep();

    const CreatureConfiguration &character() const;

    void setCharacter(const CreatureConfiguration &config);

private:
    Game *_game { nullptr };
    CharGenScreen _screen { CharGenScreen::ClassSelection };
    CreatureConfiguration _character;
    std::unique_ptr<Creature> _creature;
    Type _type  { Type::Quick };

    // Sub GUI

    std::unique_ptr<ClassSelection> _classSelection;
    std::unique_ptr<QuickOrCustom> _quickOrCustom;
    std::unique_ptr<QuickCharacterGeneration> _quick;
    std::unique_ptr<CustomCharacterGeneration> _custom;
    std::unique_ptr<PortraitSelection> _portraitSelection;
    std::unique_ptr<CharGenAbilities> _abilities;
    std::unique_ptr<CharGenSkills> _skills;
    std::unique_ptr<CharGenFeats> _feats;
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
    void loadCustom();
    void loadAbilities();
    void loadSkills();
    void loadFeats();

    // END Loading
};

} // namespace game

} // namespace reone
