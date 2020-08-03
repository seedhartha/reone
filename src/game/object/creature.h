/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <queue>

#include "../../resources/2dafile.h"
#include "../item.h"

#include "object.h"

namespace reone {

namespace game {

class Creature : public Object {
public:
    enum class ActionType {
        MoveToPoint = 0,
        Follow = 35,
        FollowLeader = 38,
        QueueEmpty = 65534
    };

    struct Action {
        ActionType type { ActionType::MoveToPoint };
        glm::vec3 point { 0.0f };
        std::shared_ptr<Object> object;
        float distance { 1.0f };

        Action(ActionType type);
        Action(ActionType, const std::shared_ptr<Object> &object, float distance);
    };

    struct Path {
        glm::vec3 destination { 0.0f };
        std::vector<glm::vec3> points;
        uint32_t timeFound { 0 };
        int pointIdx { 0 };
    };

    Creature(uint32_t id);

    void load(const resources::GffStruct &gffs);
    void load(int appearance, const glm::vec3 &position, float heading);
    void initGL() override;
    void playGreetingAnimation();
    void clearActions();
    void enqueue(const Action &action);
    void equip(const std::string &resRef);

    void saveTo(AreaState &state) const override;
    void loadState(const AreaState &state) override;

    // Setters
    void setTag(const std::string &tag);
    virtual void setMovementType(MovementType type);
    void setPath(const glm::vec3 &dest, std::vector<glm::vec3> &&points, uint32_t timeFound);
    void setPathUpdating();

    // Getters
    int appearance() const;
    std::shared_ptr<render::Texture> portrait() const;
    const Action &currentAction() const;
    const std::map<InventorySlot, std::shared_ptr<Item>> &equipment() const;
    std::shared_ptr<Path> &path();
    bool isPathUpdating() const;
    float walkSpeed() const;
    float runSpeed() const;

protected:
    MovementType _movementType { MovementType::None };

private:
    enum class ModelType {
        Creature,
        Droid,
        Character
    };

    int _appearance { 0 };
    ModelType _modelType { ModelType::Creature };
    std::shared_ptr<render::Texture> _portrait;
    std::vector<Action> _actions;
    std::map<InventorySlot, std::shared_ptr<Item>> _equipment;
    std::shared_ptr<Path> _path;
    std::atomic_bool _pathUpdating { false };
    float _walkSpeed { 0.0f };
    float _runSpeed { 0.0f };

    // Loading
    void loadBlueprint(const resources::GffStruct &gffs);
    void loadAppearance(const resources::TwoDaTable &table, int row);
    void loadPortrait(int appearance);
    void loadCharacterAppearance(const resources::TwoDaTable &table, int row);
    void loadHead(const resources::TwoDaTable &table, int row);
    void loadDefaultAppearance(const resources::TwoDaTable &table, int row);

    ModelType parseModelType(const std::string &s) const;

    const std::string &getPauseAnimation();
    const std::string &getWalkAnimation();
    const std::string &getRunAnimation();
};

} // namespace game

} // namespace reone
