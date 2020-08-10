/*
 * Copyright � 2020 Vsevolod Kremianskii
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

#include <functional>
#include <string>
#include <vector>

#include "../../resources/types.h"
#include "../../script/routine.h"
#include "../../script/types.h"
#include "../../script/variable.h"

#include "callbacks.h"

namespace reone {

namespace game {

class RoutineManager : public script::IRoutineProvider {
public:
    static RoutineManager &instance();

    void init(resources::GameVersion version, IRoutineCallbacks *callbacks);
    void deinit();

    const script::Routine &get(int index) override;

private:
    IRoutineCallbacks *_callbacks { nullptr };
    std::vector<script::Routine> _routines;

    RoutineManager() = default;
    RoutineManager(const RoutineManager &) = delete;
    ~RoutineManager();

    RoutineManager &operator=(const RoutineManager &) = delete;

    void addKotorRoutines();
    void addTslRoutines();

    void add(const std::string &name, script::VariableType retType, const std::vector<script::VariableType> &argTypes);

    void add(
        const std::string &name,
        script::VariableType retType,
        const std::vector<script::VariableType> &argTypes,
        const std::function<script::Variable(const std::vector<script::Variable>&, script::ExecutionContext &ctx)> &fn);

    script::Variable delayCommand(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable assignCommand(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getEnteringObject(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getIsPC(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getIsObjectValid(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getFirstPC(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getObjectByTag(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getLevelByClass(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable getGender(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
    script::Variable actionStartConversation(const std::vector<script::Variable> &args, script::ExecutionContext &ctx);
};

#define RoutineMan RoutineManager::instance()

} // namespace game

} // namespace reone
