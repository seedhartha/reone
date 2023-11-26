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

#include "reone/resource/format/gffreader.h"
#include "reone/scene/node/walkmesh.h"

#include "../generated/git.h"
#include "../generated/utp.h"
#include "../object.h"

namespace reone {

namespace game {

class Placeable : public Object {
public:
    Placeable(
        uint32_t id,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Object(
            id,
            ObjectType::Placeable,
            std::move(sceneName),
            game,
            services) {
    }

    void loadFromGIT(const generated::GIT_Placeable_List &git);
    void loadFromBlueprint(const std::string &resRef);

    bool hasInventory() const { return _hasInventory; }
    bool isSelectable() const override { return _usable; }
    bool isUsable() const { return _usable; }

    int appearance() const { return _appearance; }
    std::shared_ptr<scene::WalkmeshSceneNode> walkmesh() const { return _walkmesh; }

    // Scripts

    void runOnUsed(std::shared_ptr<Object> usedBy);
    void runOnInvDisturbed(std::shared_ptr<Object> triggerrer);

    // END Scripts

private:
    int _appearance {0};
    bool _hasInventory {false};
    bool _usable {false};
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

    std::shared_ptr<scene::WalkmeshSceneNode> _walkmesh;

    std::unique_ptr<generated::UTP> _utp;

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

    void loadUTP(const generated::UTP &utp);
    void loadTransformFromGIT(const generated::GIT_Placeable_List &git);

    void updateTransform() override;
};

} // namespace game

} // namespace reone
