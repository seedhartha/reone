/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include <boost/test/unit_test.hpp>

#include "../../game/selectioncontroller.h"

#include "../fixtures/game.h"
#include "../fixtures/gameobject.h"
#include "../fixtures/maininterface.h"
#include "../fixtures/scenegraph.h"
#include "../fixtures/scenenode.h"

using namespace reone::game;
using namespace reone::scene;

BOOST_AUTO_TEST_SUITE(selection_controller)

BOOST_AUTO_TEST_CASE(should_set_hovered_target_on_mouse_motion) {
    // given
    auto creature = mockCreature(2);
    auto creatureModel = mockModelSceneNode(creature.get());

    auto game = MockGame();
    auto mainInterface = MockMainInterface();

    auto sceneGraph = MockSceneGraph();
    sceneGraph.whenPickModelAtThenReturn(creatureModel.get());

    auto sut = SelectionController(game, mainInterface, sceneGraph);

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEMOTION;
    evt.motion.x = 1;
    evt.motion.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_CHECK(handled);
    BOOST_CHECK_EQUAL(1ll, game.changeCursorInvocations().size());
    BOOST_CHECK_EQUAL(static_cast<int>(CursorType::Target), static_cast<int>(game.changeCursorInvocations()[0]));
    BOOST_CHECK_EQUAL(0, creature->numHandleClickInvocations());
    BOOST_CHECK_EQUAL(creature.get(), sut.hoveredObject());
    BOOST_CHECK_EQUAL(nullptr, sut.selectedObject());
}

BOOST_AUTO_TEST_CASE(should_reset_hovered_target_on_mouse_motion_when_no_object_at_mouse_position) {
    // given
    auto creature = mockCreature(2);
    auto game = MockGame();
    auto mainInterface = MockMainInterface();
    auto sceneGraph = MockSceneGraph();

    auto sut = SelectionController(game, mainInterface, sceneGraph);
    sut.setHoveredObject(creature.get());
    sut.setSelectedObject(creature.get());

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEMOTION;
    evt.button.x = 1;
    evt.button.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_CHECK(handled);
    BOOST_CHECK_EQUAL(1ll, game.changeCursorInvocations().size());
    BOOST_CHECK_EQUAL(static_cast<int>(CursorType::Default), static_cast<int>(game.changeCursorInvocations()[0]));
    BOOST_CHECK_EQUAL(0, creature->numHandleClickInvocations());
    BOOST_CHECK_EQUAL(nullptr, sut.hoveredObject());
    BOOST_CHECK_EQUAL(creature.get(), sut.selectedObject());
}

BOOST_AUTO_TEST_CASE(should_set_selected_target_on_mouse_click) {
    // given
    auto creature = mockCreature(2);
    auto game = MockGame();
    auto mainInterface = MockMainInterface();
    auto sceneGraph = MockSceneGraph();

    auto sut = SelectionController(game, mainInterface, sceneGraph);
    sut.setHoveredObject(creature.get());

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEBUTTONDOWN;
    evt.button.x = 1;
    evt.button.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_CHECK(handled);
    BOOST_CHECK_EQUAL(0ll, game.changeCursorInvocations().size());
    BOOST_CHECK_EQUAL(0, creature->numHandleClickInvocations());
    BOOST_CHECK_EQUAL(creature.get(), sut.hoveredObject());
    BOOST_CHECK_EQUAL(creature.get(), sut.selectedObject());
}

BOOST_AUTO_TEST_CASE(should_call_handle_click_in_selected_object_on_second_mouse_click) {
    // given
    auto creature = mockCreature(2);
    auto game = MockGame();
    auto mainInterface = MockMainInterface();
    auto sceneGraph = MockSceneGraph();

    auto sut = SelectionController(game, mainInterface, sceneGraph);
    sut.setHoveredObject(creature.get());
    sut.setSelectedObject(creature.get());

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEBUTTONDOWN;
    evt.button.x = 1;
    evt.button.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_CHECK(handled);
    BOOST_CHECK_EQUAL(0ll, game.changeCursorInvocations().size());
    BOOST_CHECK_EQUAL(1, creature->numHandleClickInvocations());
    BOOST_CHECK_EQUAL(creature.get(), sut.hoveredObject());
    BOOST_CHECK_EQUAL(creature.get(), sut.selectedObject());
}

BOOST_AUTO_TEST_CASE(should_reset_selected_target_on_mouse_click_when_no_object_at_mouse_position) {
    // given
    auto creature = mockCreature(2);
    auto game = MockGame();
    auto mainInterface = MockMainInterface();
    auto sceneGraph = MockSceneGraph();

    auto sut = SelectionController(game, mainInterface, sceneGraph);
    sut.setSelectedObject(creature.get());

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEBUTTONDOWN;
    evt.button.x = 1;
    evt.button.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_CHECK(handled);
    BOOST_CHECK_EQUAL(0ll, game.changeCursorInvocations().size());
    BOOST_CHECK_EQUAL(0, creature->numHandleClickInvocations());
    BOOST_CHECK_EQUAL(nullptr, sut.hoveredObject());
    BOOST_CHECK_EQUAL(nullptr, sut.selectedObject());
}

BOOST_AUTO_TEST_SUITE_END()
