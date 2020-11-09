/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../../resource/gfffile.h"

namespace reone {

namespace audio {

class SoundInstance;

}

namespace game {

class SoundBlueprint;

class Sound : public SpatialObject {
public:
    Sound(uint32_t id, scene::SceneGraph *sceneGraph);

    void load(const resource::GffStruct &gffs);

    void update(float dt) override;

    bool isActive() const;
    bool isAudible() const;

    std::shared_ptr<SoundBlueprint> blueprint() const;
    int priority() const;

    void setAudible(bool audible);

private:
    std::shared_ptr<SoundBlueprint> _blueprint;
    bool _active { false };
    bool _audible { false };
    int _priority { 0 };
    int _soundIdx { -1 };
    std::shared_ptr<audio::SoundInstance> _sound;

    void playSound(const std::string &resRef);
};

} // namespace game

} // namespace reone
