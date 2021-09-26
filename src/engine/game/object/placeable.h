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

#include "../../resource/format/gffreader.h"

#include "spatial.h"

namespace reone {

namespace game {

class Game;
class ObjectFactory;

class Placeable : public SpatialObject {
public:
    Placeable(
        uint32_t id,
        Game *game,
        ObjectFactory *objectFactory,
        scene::SceneGraph *sceneGraph);

    void loadFromGIT(const resource::GffStruct &gffs);
    void loadFromBlueprint(const std::string &resRef);

    bool isSelectable() const override;

    bool hasInventory() const { return _hasInventory; }

    bool isUsable() const { return _usable; }

    std::shared_ptr<graphics::Walkmesh> getWalkmesh() const override;

    // Scripts

    void runOnUsed(std::shared_ptr<SpatialObject> usedBy);
    void runOnInvDisturbed(std::shared_ptr<SpatialObject> triggerrer);

    // END Scripts

private:
    int _appearance {0};
    bool _hasInventory {false};
    bool _usable {false};
    std::shared_ptr<graphics::Walkmesh> _walkmesh;
    Faction _faction {Faction::Invalid};
    bool _keyRequired {false};
    bool _lockable {false};
    bool _locked {false};
    int _openLockDC {0};
    int _animationState {0};
    int _hardness {0};
    int _fortitude {0};
    bool _partyInteract {false};
    bool _static {false};

    // Scripts

    std::string _onUsed;
    std::string _onInvDisturbed;
    std::string _onClosed;
    std::string _onDamaged;
    std::string _onDeath;
    std::string _onLock;
    std::string _onMeleeAttacked;
    std::string _onOpen;
    std::string _onSpellCastAt;
    std::string _onUnlock;
    std::string _onEndDialogue;

    // END Scripts

    void loadTransformFromGIT(const resource::GffStruct &gffs);

    void loadUTP(const resource::GffStruct &utp);
};

} // namespace game

} // namespace reone
