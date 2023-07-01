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

#include "reone/script/types.h"
#include "reone/script/variable.h"

namespace reone {

namespace script {

struct ExecutionContext;

} // namespace script

namespace game {

struct RoutineContext;

class Area;
class Creature;
class Door;
class Effect;
class Event;
class Item;
class Location;
class Object;
class Placeable;
class Sound;
class Talent;

std::shared_ptr<Object> getCaller(const RoutineContext &ctx);
std::shared_ptr<Object> getTriggerrer(const RoutineContext &ctx);

int getInt(const std::vector<script::Variable> &args, int index);
float getFloat(const std::vector<script::Variable> &args, int index);
std::string getString(const std::vector<script::Variable> &args, int index);
glm::vec3 getVector(const std::vector<script::Variable> &args, int index);
std::shared_ptr<Object> getObject(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);
std::shared_ptr<Effect> getEffect(const std::vector<script::Variable> &args, int index);
std::shared_ptr<Event> getEvent(const std::vector<script::Variable> &args, int index);
std::shared_ptr<Location> getLocationArgument(const std::vector<script::Variable> &args, int index);
std::shared_ptr<Talent> getTalent(const std::vector<script::Variable> &args, int index);
std::shared_ptr<script::ExecutionContext> getAction(const std::vector<script::Variable> &args, int index);

int getIntOrElse(const std::vector<script::Variable> &args, int index, int defValue);
float getFloatOrElse(const std::vector<script::Variable> &args, int index, float defValue);
std::string getStringOrElse(const std::vector<script::Variable> &args, int index, std::string defValue);
glm::vec3 getVectorOrElse(const std::vector<script::Variable> &args, int index, glm::vec3 defValue);
std::shared_ptr<Object> getObjectOrNull(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);
std::shared_ptr<Object> getObjectOrCaller(const std::vector<script::Variable> &args, int index, const RoutineContext &ctx);

std::shared_ptr<Creature> checkCreature(const std::shared_ptr<Object> &object);
std::shared_ptr<Door> checkDoor(const std::shared_ptr<Object> &object);
std::shared_ptr<Placeable> checkPlaceable(const std::shared_ptr<Object> &object);
std::shared_ptr<Item> checkItem(const std::shared_ptr<Object> &object);
std::shared_ptr<Sound> checkSound(const std::shared_ptr<Object> &object);
std::shared_ptr<Area> checkArea(const std::shared_ptr<Object> &object);

} // namespace game

} // namespace reone
