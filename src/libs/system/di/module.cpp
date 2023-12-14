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

#include "reone/system/di/module.h"

namespace reone {

static constexpr int kNumThreadPoolThreads = 2;

void SystemModule::init() {
    _threadPool = std::make_unique<ThreadPool>(kNumThreadPoolThreads);
    _threadPool->init();

    _services = std::make_unique<SystemServices>(_clock, *_threadPool);
}

void SystemModule::deinit() {
    _services.reset();

    _threadPool.reset();
}

} // namespace reone
