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

#include "../../../common/collectionutil.h"
#include "../../../script/exception/argument.h"
#include "../../../script/types.h"
#include "../../../script/variable.h"

namespace reone {

namespace script {

struct ExecutionContext;

} // namespace script

namespace game {

class Area;
class Door;
class Creature;
class Effect;
class Event;
class Item;
class Location;
class Object;
class Placeable;
class Sound;
class SpatialObject;
class Talent;

} // namespace game

namespace kotor {

struct RoutineContext;

std::shared_ptr<game::Object> getCaller(const RoutineContext &ctx);
std::shared_ptr<game::Object> getTriggerrer(const RoutineContext &ctx);

int getInt(const std::vector<script::Variable> &args, int index);
float getFloat(const std::vector<script::Variable> &args, int index);
std::string getString(const std::vector<script::Variable> &args, int index);
glm::vec3 getVector(const std::vector<script::Variable> &args, int index);
std::shared_ptr<game::Object> getObject(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);
std::shared_ptr<game::Effect> getEffect(const std::vector<script::Variable> &args, int index);
std::shared_ptr<game::Event> getEvent(const std::vector<script::Variable> &args, int index);
std::shared_ptr<game::Location> getLocationArgument(const std::vector<script::Variable> &args, int index);
std::shared_ptr<game::Talent> getTalent(const std::vector<script::Variable> &args, int index);
std::shared_ptr<script::ExecutionContext> getAction(const std::vector<script::Variable> &args, int index);

int getIntOrElse(const std::vector<script::Variable> &args, int index, int defValue);
float getFloatOrElse(const std::vector<script::Variable> &args, int index, float defValue);
std::string getStringOrElse(const std::vector<script::Variable> &args, int index, std::string defValue);
glm::vec3 getVectorOrElse(const std::vector<script::Variable> &args, int index, glm::vec3 defValue);

bool getIntAsBool(const std::vector<script::Variable> &args, int index);
bool getIntAsBoolOrElse(const std::vector<script::Variable> &args, int index, bool defValue);

std::shared_ptr<game::Object> getObjectOrNull(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);
std::shared_ptr<game::Object> getObjectOrCaller(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

std::shared_ptr<game::SpatialObject> getCallerAsSpatialObject(const RoutineContext &ctx);
std::shared_ptr<game::SpatialObject> getObjectAsSpatialObject(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);
std::shared_ptr<game::SpatialObject> getObjectOrCallerAsSpatialObject(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

std::shared_ptr<game::Creature> getCallerAsCreature(const RoutineContext &ctx);
std::shared_ptr<game::Creature> getObjectAsCreature(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);
std::shared_ptr<game::Creature> getObjectOrCallerAsCreature(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

std::shared_ptr<game::Door> getObjectAsDoor(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

std::shared_ptr<game::Placeable> getObjectAsPlaceable(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

std::shared_ptr<game::Item> getObjectAsItem(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);
std::shared_ptr<game::Item> getObjectAsItemOrNull(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

std::shared_ptr<game::Sound> getObjectAsSound(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

std::shared_ptr<game::Area> getObjectAsArea(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);
std::shared_ptr<game::Area> getObjectAsAreaOrCallerArea(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

inline void throwIfOutOfRange(const std::vector<script::Variable> &args, int index) {
    if (isOutOfRange(args, index)) {
        throw script::ArgumentException(str(boost::format("Argument index is out of range: %d/%d") % index % static_cast<int>(args.size())));
    }
}

inline void throwIfUnexpectedType(script::VariableType expected, script::VariableType actual) {
    if (actual != expected) {
        throw script::ArgumentException(str(boost::format("Expected argument of type %d, but got %d") % static_cast<int>(expected) % static_cast<int>(actual)));
    }
}

template <class T>
inline T getIntAsEnum(const std::vector<script::Variable> &args, int index) {
    throwIfOutOfRange(args, index);
    throwIfUnexpectedType(script::VariableType::Int, args[index].type);
    return static_cast<T>(args[index].intValue);
}

template <class T>
inline T getIntAsEnumOrElse(const std::vector<script::Variable> &args, int index, T defValue) {
    if (isOutOfRange(args, index)) {
        return defValue;
    }
    throwIfUnexpectedType(script::VariableType::Int, args[index].type);
    return static_cast<T>(args[index].intValue);
}

} // namespace kotor

} // namespace reone
