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

#include "area.h"

#include "../../common/log.h"

#include "../enginetype/location.h"
#include "../game.h"

using namespace std;

namespace reone {

namespace game {

shared_ptr<SpatialObject> Area::getNearestObject(const glm::vec3 &origin, int nth, const std::function<bool(const std::shared_ptr<SpatialObject> &)> &predicate) {
    vector<pair<shared_ptr<SpatialObject>, float>> candidates;

    for (auto &object : _objects) {
        if (predicate(object)) {
            candidates.push_back(make_pair(object, object->getDistanceTo2(origin)));
        }
    }
    sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) { return left.second < right.second; });

    int candidateCount = static_cast<int>(candidates.size());
    if (nth >= candidateCount) {
        debug(boost::format("Area: getNearestObject: nth is out of bounds: %d/%d") % nth % candidateCount, 2);
        return nullptr;
    }

    return candidates[nth].first;
}

shared_ptr<Creature> Area::getNearestCreature(const std::shared_ptr<SpatialObject> &target, const SearchCriteriaList &criterias, int nth) {
    vector<pair<shared_ptr<Creature>, float>> candidates;

    for (auto &object : getObjectsByType(ObjectType::Creature)) {
        auto creature = static_pointer_cast<Creature>(object);
        if (matchesCriterias(*creature, criterias, target)) {
            float distance2 = creature->getDistanceTo2(*target);
            candidates.push_back(make_pair(move(creature), distance2));
        }
    }

    sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });

    return nth < candidates.size() ? candidates[nth].first : nullptr;
}

bool Area::matchesCriterias(const Creature &creature, const SearchCriteriaList &criterias, std::shared_ptr<SpatialObject> target) const {
    for (auto &criteria : criterias) {
        switch (criteria.first) {
            case CreatureType::Reputation: {
                auto reputation = static_cast<ReputationType>(criteria.second);
                switch (reputation) {
                    case ReputationType::Friend:
                        if (!target || !_game->reputes().getIsFriend(creature, *static_pointer_cast<Creature>(target))) return false;
                        break;
                    case ReputationType::Enemy:
                        if (!target || !_game->reputes().getIsEnemy(creature, *static_pointer_cast<Creature>(target))) return false;
                        break;
                    case ReputationType::Neutral:
                        if (!target || !_game->reputes().getIsNeutral(creature, *static_pointer_cast<Creature>(target))) return false;
                        break;
                    default:
                        break;
                }
                break;
            }
            case CreatureType::Perception: {
                if (!target) return false;

                bool seen = creature.perception().seen.count(target) > 0;
                bool heard = creature.perception().heard.count(target) > 0;
                bool matches = false;
                auto perception = static_cast<PerceptionType>(criteria.second);
                switch (perception) {
                    case PerceptionType::SeenAndHeard:
                        matches = seen && heard;
                        break;
                    case PerceptionType::NotSeenAndNotHeard:
                        matches = !seen && !heard;
                        break;
                    case PerceptionType::HeardAndNotSeen:
                        matches = heard && !seen;
                        break;
                    case PerceptionType::SeenAndNotHeard:
                        matches = seen && !heard;
                        break;
                    case PerceptionType::NotHeard:
                        matches = !heard;
                        break;
                    case PerceptionType::Heard:
                        matches = heard;
                        break;
                    case PerceptionType::NotSeen:
                        matches = !seen;
                        break;
                    case PerceptionType::Seen:
                        matches = seen;
                        break;
                    default:
                        break;
                }
                if (!matches) return false;
                break;
            }
            default:
                // TODO: implement other criterias
                break;
        }
    }

    return true;
}

shared_ptr<Creature> Area::getNearestCreatureToLocation(const Location &location, const SearchCriteriaList &criterias, int nth) {
    vector<pair<shared_ptr<Creature>, float>> candidates;

    for (auto &object : getObjectsByType(ObjectType::Creature)) {
        auto creature = static_pointer_cast<Creature>(object);
        if (matchesCriterias(*creature, criterias)) {
            float distance2 = creature->getDistanceTo2(location.position());
            candidates.push_back(make_pair(move(creature), distance2));
        }
    }

    sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });

    return nth < candidates.size() ? candidates[nth].first : nullptr;
}

} // namespace game

} // namespace reone
