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

namespace reone {

namespace resource {

class TwoDAs;

}

namespace game {

class Creature;

class IReputes {
public:
    virtual ~IReputes() = default;

    virtual bool getIsEnemy(const Creature &left, const Creature &right) const = 0;
    virtual bool getIsFriend(const Creature &left, const Creature &right) const = 0;
    virtual bool getIsNeutral(const Creature &left, const Creature &right) const = 0;
};

class Reputes : public IReputes, boost::noncopyable {
public:
    Reputes(resource::TwoDAs &twoDas) :
        _twoDas(twoDas) {
    }

    void init();

    bool getIsEnemy(const Creature &left, const Creature &right) const override;
    bool getIsFriend(const Creature &left, const Creature &right) const override;
    bool getIsNeutral(const Creature &left, const Creature &right) const override;

private:
    resource::TwoDAs &_twoDas;

    int getRepute(const Creature &left, const Creature &right) const;
};

} // namespace game

} // namespace reone
