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

/** @file
 *  Implementation of talent-related routines.
 */

#include "../declarations.h"

#include "../../../../game/script/routine/argutil.h"
#include "../../../../game/script/routine/context.h"
#include "../../../../game/script/routine/objectutil.h"
#include "../../../../game/talent.h"
#include "../../../../script/exception/argument.h"
#include "../../../../script/exception/notimpl.h"
#include "../../../../script/types.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

Variable talentSpell(const vector<Variable> &args, const RoutineContext &ctx) {
    int spell = getInt(args, 0);
    auto talent = make_shared<Talent>(TalentType::Spell, spell);

    return Variable::ofTalent(move(talent));
}

Variable talentFeat(const vector<Variable> &args, const RoutineContext &ctx) {
    int feat = getInt(args, 0);
    auto talent = make_shared<Talent>(TalentType::Feat, feat);

    return Variable::ofTalent(move(talent));
}

Variable talentSkill(const vector<Variable> &args, const RoutineContext &ctx) {
    int skill = getInt(args, 0);
    auto talent = make_shared<Talent>(TalentType::Skill, skill);

    return Variable::ofTalent(move(talent));
}

Variable getIsTalentValid(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto talent = getTalent(args, 0);
        return Variable::ofInt(1);
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getTypeFromTalent(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto talent = getTalent(args, 0);
        auto type = talent->type();

        return Variable::ofInt(static_cast<int>(type));
    } catch (const ArgumentException &) {
        return Variable::ofInt(static_cast<int>(TalentType::Invalid));
    }
}

Variable getIdFromTalent(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getCategoryFromTalent(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace kotor

} // namespace reone
