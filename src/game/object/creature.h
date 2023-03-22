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

#pragma once

#include "../object.h"

namespace reone {

namespace resource {

class Gff;

}

namespace game {

class IItem;

class Creature : public Object {
public:
    enum class State {
        Pause,
        Walk,
        Run
    };

    Creature(
        uint32_t id,
        IGame &game,
        IObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Object(
            id,
            ObjectType::Creature,
            game,
            objectFactory,
            gameSvc,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
    }

    void loadFromGit(const resource::Gff &git);
    void loadFromUtc(const std::string &templateResRef);

    /**
     * @return true when destination have been reached
     */
    virtual bool moveTo(Object &other, bool run, float range, float delta);

    /**
     * @return true when destination have been reached
     */
    virtual bool moveTo(const glm::vec2 &destination, bool run, float range, float delta);

    bool moveForward(float delta);

    void setState(State state) {
        _state = state;
    }

    // Object

    void update(float delta) override;

    void handleClick(Object &clicker) override;

    glm::vec3 targetWorldCoords() const override;

    // END Object

    // Equipment

    bool equip(IItem &item);
    bool equip(int slot, IItem &item);

    void unequip(int slot);

    // END Equipment

private:
    enum class ModelType : char {
        FullBody = 'F',
        BodyOnly = 'B',
        Critter = 'L',
        UnarmedCritter = 'S'
    };

    struct PlottedPath {
        glm::vec2 destination {0.0f};
        std::queue<glm::vec2> points;
        float duration {0.0f};
    } _plottedPath;

    State _state {State::Pause};

    ModelType _modelType {ModelType::FullBody};
    std::string _conversation;

    std::map<int, IItem *> _equipment;
};

} // namespace game

} // namespace reone