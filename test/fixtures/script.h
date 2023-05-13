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

#include "reone/script/di/services.h"
#include "reone/script/executioncontext.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"

namespace reone {

namespace script {

class MockRoutine : public Routine {
public:
    typedef std::tuple<std::vector<Variable>, ExecutionContext> InvokeInvocation;

    MockRoutine(
        std::string name,
        VariableType retType,
        Variable retValue,
        std::vector<VariableType> argTypes) :
        Routine(
            std::move(name),
            retType,
            retValue,
            std::move(argTypes),
            [retValue](auto &args, auto &ctx) { return retValue; }) {
    }

    Variable invoke(const std::vector<Variable> &args, ExecutionContext &ctx) override {
        _invokeInvocations.push_back(std::make_tuple(args, ctx));
        return Routine::invoke(args, ctx);
    }

    const std::vector<InvokeInvocation> &invokeInvocations() const {
        return _invokeInvocations;
    }

private:
    std::vector<InvokeInvocation> _invokeInvocations;
};

class MockRoutines : public IRoutines, boost::noncopyable {
public:
    Routine &get(int index) override {
        if (_routines.count(index) == 0) {
            throw std::out_of_range("index is out of range: " + std::to_string(index));
        }
        return *_routines.at(index);
    }

    int getNumRoutines() const override {
        return _routines.size();
    }

    int getIndexByName(const std::string &name) const override {
        for (auto &pair : _routines) {
            if (pair.second->name() == name) {
                return pair.first;
            }
        }
        return -1;
    }

    void add(int index, std::shared_ptr<Routine> routine) {
        _routines[index] = std::move(routine);
    }

private:
    std::map<int, std::shared_ptr<Routine>> _routines;
};

class MockScripts : public IScripts, boost::noncopyable {
public:
    void invalidate() override {}

    std::shared_ptr<ScriptProgram> get(const std::string &key) override {
        return nullptr;
    }
};

class TestScriptModule : boost::noncopyable {
public:
    void init() {
        _scripts = std::make_unique<MockScripts>();

        _services = std::make_unique<ScriptServices>(*_scripts);
    }

    ScriptServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockScripts> _scripts;

    std::unique_ptr<ScriptServices> _services;
};

} // namespace script

} // namespace reone
