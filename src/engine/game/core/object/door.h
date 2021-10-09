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

#include "spatial.h"

#include "../../../resource/format/gffreader.h"

namespace reone {

namespace game {

class Door : public SpatialObject {
public:
    Door(
        uint32_t id,
        Game *game,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        FootstepSounds &footstepSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        resource::Resources &resources,
        resource::Strings &strings,
        scene::SceneGraph &sceneGraph) :
        SpatialObject(
            id,
            ObjectType::Door,
            game,
            actionFactory,
            classes,
            combat,
            footstepSounds,
            objectFactory,
            party,
            portraits,
            reputes,
            scriptRunner,
            soundSets,
            surfaces,
            audioFiles,
            audioPlayer,
            context,
            meshes,
            models,
            shaders,
            textures,
            walkmeshes,
            resources,
            strings,
            sceneGraph) {
    }

    void loadFromGIT(const resource::GffStruct &gffs);
    void loadFromBlueprint(const std::string &resRef);

    bool isSelectable() const override;

    void open(const std::shared_ptr<Object> &triggerrer);
    void close(const std::shared_ptr<Object> &triggerrer);

    bool isLocked() const { return _locked; }
    bool isStatic() const { return _static; }
    bool isKeyRequired() const { return _keyRequired; }

    std::shared_ptr<graphics::Walkmesh> getWalkmesh() const override;

    const std::string &getOnOpen() const { return _onOpen; }
    const std::string &getOnFailToOpen() const { return _onFailToOpen; }

    int genericType() const { return _genericType; }
    const std::string &linkedToModule() const { return _linkedToModule; }
    const std::string &linkedTo() const { return _linkedTo; }
    const std::string &transitionDestin() const { return _transitionDestin; }

    void setLocked(bool locked);

private:
    bool _locked {false};
    int _genericType {0};
    bool _static {false};
    bool _keyRequired {false};
    std::string _linkedToModule;
    std::string _linkedTo;
    int _linkedToFlags {0};
    std::string _transitionDestin;
    Faction _faction {Faction::Invalid};
    int _openLockDC {0};
    int _hardness {0};
    int _fortitude {0};
    bool _lockable {false};
    std::string _keyName;

    // Scripts

    std::string _onOpen;
    std::string _onFailToOpen;
    std::string _onClick;
    std::string _onClosed;
    std::string _onDamaged;
    std::string _onLock;
    std::string _onUnlock;
    std::string _onMeleeAttacked;
    std::string _onSpellCastAt;

    // END Scripts

    // Walkmeshes

    std::shared_ptr<graphics::Walkmesh> _closedWalkmesh;
    std::shared_ptr<graphics::Walkmesh> _open1Walkmesh;
    std::shared_ptr<graphics::Walkmesh> _open2Walkmesh;

    // END Walkmeshes

    void loadTransformFromGIT(const resource::GffStruct &gffs);

    void loadUTD(const resource::GffStruct &utd);
};

} // namespace game

} // namespace reone
