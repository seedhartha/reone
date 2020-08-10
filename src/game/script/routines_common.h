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

#include <vector>

#include "../../script/types.h"
#include "../../script/variable.h"

namespace reone {

namespace game {

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

} // namespace game

} // namespace reone
