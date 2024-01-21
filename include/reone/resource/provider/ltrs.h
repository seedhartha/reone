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

#include "reone/system/cache.h"
#include "reone/system/stream/memoryinput.h"

#include "../format/ltrreader.h"
#include "../ltr.h"
#include "../resources.h"
#include "../resref.h"

namespace reone {

namespace resource {

class ILtrs {
public:
    virtual ~ILtrs() = default;

    virtual std::shared_ptr<Ltr> get(const ResRef &resRef) = 0;
};

class Ltrs : public ILtrs, boost::noncopyable {
public:
    Ltrs(Resources &resources) :
        _resources(resources) {
    }

    std::shared_ptr<Ltr> get(const ResRef &resRef) override {
        std::lock_guard<std::mutex> lock {_mutex};
        return _cache.getOrAdd(resRef, std::bind(&Ltrs::getFromResources, this, resRef));
    }

private:
    Resources &_resources;

    Cache<ResRef, Ltr> _cache;
    std::mutex _mutex;

    std::shared_ptr<Ltr> getFromResources(const ResRef &resRef) {
        auto res = _resources.get(ResourceId(resRef, ResType::Ltr));
        auto stream = MemoryInputStream(res.data);
        auto reader = LtrReader(stream);
        return reader.load();
    }
};

} // namespace resource

} // namespace reone
