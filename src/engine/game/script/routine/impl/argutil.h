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

#include "../../../../common/collectionutil.h"
#include "../../../../script/executioncontext.h"

namespace reone {

namespace script {

struct Variable;

}

namespace game {

class Door;
class Creature;
class Effect;
class Event;
class Game;
class Item;
class Location;
class Object;
class Sound;
class SpatialObject;
class Talent;

bool getBool(const std::vector<script::Variable> &args, int index);
int getInt(const std::vector<script::Variable> &args, int index);
float getFloat(const std::vector<script::Variable> &args, int index);
std::string getString(const std::vector<script::Variable> &args, int index);
glm::vec3 getVector(const std::vector<script::Variable> &args, int index);

bool getBoolOrElse(const std::vector<script::Variable> &args, int index, bool defValue);
int getIntOrElse(const std::vector<script::Variable> &args, int index, int defValue);
float getFloatOrElse(const std::vector<script::Variable> &args, int index, float defValue);
std::string getStringOrElse(const std::vector<script::Variable> &args, int index, std::string defValue);
glm::vec3 getVectorOrElse(const std::vector<script::Variable> &args, int index, glm::vec3 defValue);

std::shared_ptr<Object> getCaller(Game &game, script::ExecutionContext &ctx);
std::shared_ptr<SpatialObject> getCallerAsSpatial(Game &game, script::ExecutionContext &ctx);
std::shared_ptr<Creature> getCallerAsCreature(Game &game, script::ExecutionContext &ctx);
std::shared_ptr<Object> getTriggerrer(Game &game, script::ExecutionContext &ctx);

std::shared_ptr<Object> getObject(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<Object> getObjectOrCaller(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<SpatialObject> getSpatialObject(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<SpatialObject> getSpatialObjectOrCaller(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<Creature> getCreature(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<Creature> getCreatureOrCaller(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<Door> getDoor(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<Item> getItem(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<Sound> getSound(Game &game, const std::vector<script::Variable> &args, int index, script::ExecutionContext &ctx);
std::shared_ptr<Effect> getEffect(const std::vector<script::Variable> &args, int index);
std::shared_ptr<Event> getEvent(const std::vector<script::Variable> &args, int index);
std::shared_ptr<Location> getLocationEngineType(const std::vector<script::Variable> &args, int index);
std::shared_ptr<Talent> getTalent(const std::vector<script::Variable> &args, int index);
std::shared_ptr<script::ExecutionContext> getAction(const std::vector<script::Variable> &args, int index);

template <class T>
inline T getEnum(const std::vector<script::Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw std::out_of_range("index is out of range");
    }
    return static_cast<T>(args[index].intValue);
}

template <class T>
inline T getEnum(const std::vector<script::Variable> &args, int index, T defValue) {
    return isOutOfRange(args, index) ?
        std::move(defValue) :
        static_cast<T>(args[index].intValue);
}

} // namespace game

} // namespace reone
