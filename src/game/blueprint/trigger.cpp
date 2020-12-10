/*
 * Copyright (c) 2020 The reone project contributors
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

#include "trigger.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../object/trigger.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

TriggerBlueprint::TriggerBlueprint(const string &resRef, const shared_ptr<GffStruct> &utt) :
    _resRef(resRef),
    _utt(utt) {

    if (!utt) {
        throw invalid_argument("utt must not be null");
    }
}

void TriggerBlueprint::load(Trigger &trigger) {
    trigger._tag = boost::to_lower_copy(_utt->getString("Tag"));
    trigger._onEnter = boost::to_lower_copy(_utt->getString("ScriptOnEnter"));
    trigger._onExit = boost::to_lower_copy(_utt->getString("ScriptOnExit"));
}

} // namespace resource

} // namespace reone
