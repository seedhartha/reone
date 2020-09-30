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

#include <atomic>
#include <queue>

#include "../../resources/2dafile.h"
#include "../../resources/gfffile.h"
#include "../../script/types.h"

#include "../blueprint/creature.h"

#include "item.h"
#include "spatial.h"

namespace reone {

namespace game {

class ObjectFactory;

class Creature : public SpatialObject {
public:
    enum class ActionType {
        MoveToPoint = 0,
        OpenDoor = 5,
        CloseDoor = 6,
        Follow = 35,
        FollowLeader = 38,
        QueueEmpty = 65534,

        DoCommand = 0x1000,
        StartConversation = 0x1001,
        PauseConversation = 0x1002,
        ResumeConversation = 0x1003
    };

    struct Action {
        ActionType type { ActionType::MoveToPoint };
        glm::vec3 point { 0.0f };
        std::shared_ptr<Object> object;
        float distance { 1.0f };
        script::ExecutionContext context;
        std::string resRef;

        Action(ActionType type);
        Action(ActionType type, const std::shared_ptr<Object> &object, float distance);
        Action(ActionType type, const script::ExecutionContext &ctx);
    };

    struct Path {
        glm::vec3 destination { 0.0f };
        std::vector<glm::vec3> points;
        uint32_t timeFound { 0 };
        int pointIdx { 0 };
    };

    Creature(uint32_t id, ObjectFactory *objectFactory, render::SceneGraph *sceneGraph);

    void load(const resources::GffStruct &gffs);
    void load(const CreatureConfiguration &config);

    Gender gender() const;
    int getClassLevel(ClassType clazz) const;
    int appearance() const;
    std::shared_ptr<render::Texture> portrait() const;
    std::string conversation() const;
    bool hasActions() const;
    float walkSpeed() const;
    float runSpeed() const;
    glm::vec3 selectablePosition() const override;

    void setTag(const std::string &tag);
    virtual void setMovementType(MovementType type);
    virtual void setTalking(bool talking);

    // Animations

    void playDefaultAnimation();
    void playGreetingAnimation();
    void playTalkAnimation();

    // END Animations

    // Actions

    void clearActions();
    void enqueueAction(Action action);
    void popCurrentAction();

    const Action &currentAction() const;

    // END Actions

    // Load/save

    void saveTo(AreaState &state) const override;
    void loadState(const AreaState &state) override;

    // END Load/save

    // Equipment

    void equip(const std::string &resRef);
    void equip(InventorySlot slot, const std::shared_ptr<Item> &item);
    void unequip(const std::shared_ptr<Item> &item);

    std::shared_ptr<Item> getEquippedItem(InventorySlot slot) const;

    const std::map<InventorySlot, std::shared_ptr<Item>> &equipment() const;

    // END Equipment

    // Pathfinding

    std::shared_ptr<Path> &path();

    void setPath(const glm::vec3 &dest, std::vector<glm::vec3> &&points, uint32_t timeFound);
    void clearPath();

    // END Pathfinding

protected:
    MovementType _movementType { MovementType::None };
    bool _talking { false };

private:
    enum class ModelType {
        Creature,
        Droid,
        Character
    };

    ObjectFactory *_objectFactory { nullptr };
    CreatureConfiguration _config;
    std::shared_ptr<CreatureBlueprint> _blueprint;
    ModelType _modelType { ModelType::Creature };
    std::shared_ptr<render::Texture> _portrait;
    std::map<InventorySlot, std::shared_ptr<Item>> _equipment;
    std::list<Action> _actions;
    std::shared_ptr<Path> _path;
    float _walkSpeed { 0.0f };
    float _runSpeed { 0.0f };

    // Loading
    void loadBlueprint(const std::string &resRef);
    void loadAppearance(const resources::TwoDaTable &table, int row);
    void loadPortrait(int appearance);

    ModelType parseModelType(const std::string &s) const;
    void updateAppearance();
    std::string getBodyModelName() const;
    std::string getBodyTextureName() const;
    std::string getHeadModelName() const;
    std::string getWeaponModelName(InventorySlot slot) const;

    const std::string &getPauseAnimation();
    const std::string &getWalkAnimation();
    const std::string &getRunAnimation();
};

} // namespace game

} // namespace reone
