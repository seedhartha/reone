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

#include <gmock/gmock.h>

#include "reone/script/di/services.h"
#include "reone/script/executioncontext.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"

namespace reone {

namespace script {

class MockRoutine : public Routine {
public:
    usinsg InvokeInvocation = std::tuple<std::vector<Variable>, ExecutionContext>;

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
    MOCK_METHOD(Routine &, get, (int index), (override));
    MOCK_METHOD(int, getNumRoutines, (), (const override));
    MOCK_METHOD(int, getIndexByName, (const std::string &name), (const override));
};

class MockScripts : public IScripts, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<ScriptProgram>, get, (const std::string &key), (override));
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
