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

#include "reone/audio/source.h"

#include "../node.h"

namespace reone {

namespace scene {

class SoundSceneNode : public SceneNode {
public:
    SoundSceneNode(
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        SceneNode(
            SceneNodeType::Sound,
            sceneGraph,
            graphicsSvc,
            resourceSvc) {
    }

    void update(float dt) override;

    void playSound(const std::string &resRef, float gain, bool positional, bool loop);

    bool isSoundPlaying() const;

    int priority() const { return _priority; }
    float maxDistance() const { return _maxDistance; }
    bool auidible() const { return _audible; }

    void setPriority(int priority) { _priority = priority; }
    void setMaxDistance(float distance) { _maxDistance = distance; }
    void setAudible(bool audible);

private:
    int _priority {0};
    float _maxDistance {0.0f};
    bool _audible {false};

    std::shared_ptr<audio::AudioSource> _source;
};

} // namespace scene

} // namespace reone
