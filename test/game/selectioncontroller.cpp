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

#include "reone/game/selectioncontroller.h"

#include "../fixtures/functional.h"
#include "../fixtures/game.h"
#include "../fixtures/scene.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::scene;

BOOST_AUTO_TEST_SUITE(selection_controller)

BOOST_AUTO_TEST_CASE(should_set_hovered_target_on_mouse_motion) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();
    auto creature = game->mockCreature();

    auto &scene = test.sceneMockByName(kSceneMain);
    auto creatureModel = scene.newModel(*static_cast<graphics::Model *>(nullptr), ModelUsage::Creature);
    creatureModel->setUser(*creature);
    scene.whenPickModelAtThenReturn(creatureModel.get());

    auto mainInterface = MockMainInterface();

    auto sut = SelectionController(*game, mainInterface, scene);

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEMOTION;
    evt.motion.x = 1;
    evt.motion.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_TEST(handled);
    BOOST_TEST(1ll == game->changeCursorInvocations().size());
    BOOST_TEST(static_cast<int>(CursorType::Target) == static_cast<int>(get<0>(game->changeCursorInvocations()[0])));
    BOOST_TEST(creature->handleClickInvocations().empty());
    BOOST_TEST(creature.get() == sut.hoveredObject());
    BOOST_TEST(nullptr == sut.selectedObject());
}

BOOST_AUTO_TEST_CASE(should_reset_hovered_target_on_mouse_motion_when_no_object_at_mouse_position) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();
    auto creature = game->mockCreature();
    auto mainInterface = MockMainInterface();
    auto &scene = test.sceneMockByName(kSceneMain);

    auto sut = SelectionController(*game, mainInterface, scene);
    sut.setHoveredObject(creature.get());
    sut.setSelectedObject(creature.get());

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEMOTION;
    evt.button.x = 1;
    evt.button.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_TEST(handled);
    BOOST_TEST(1ll == game->changeCursorInvocations().size());
    BOOST_TEST(static_cast<int>(CursorType::Default) == static_cast<int>(get<0>(game->changeCursorInvocations()[0])));
    BOOST_TEST(creature->handleClickInvocations().empty());
    BOOST_TEST(nullptr == sut.hoveredObject());
    BOOST_TEST(creature.get() == sut.selectedObject());
}

BOOST_AUTO_TEST_CASE(should_set_selected_target_on_mouse_click) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();
    auto creature = game->mockCreature();
    auto mainInterface = MockMainInterface();
    auto &scene = test.sceneMockByName(kSceneMain);

    auto sut = SelectionController(*game, mainInterface, scene);
    sut.setHoveredObject(creature.get());

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEBUTTONDOWN;
    evt.button.x = 1;
    evt.button.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_TEST(handled);
    BOOST_TEST(0ll == game->changeCursorInvocations().size());
    BOOST_TEST(creature->handleClickInvocations().empty());
    BOOST_TEST(creature.get() == sut.hoveredObject());
    BOOST_TEST(creature.get() == sut.selectedObject());
}

BOOST_AUTO_TEST_CASE(should_call_handle_click_in_selected_object_on_second_mouse_click) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();
    auto creature = game->mockCreature();
    auto mainInterface = MockMainInterface();
    auto &scene = test.sceneMockByName(kSceneMain);

    auto sut = SelectionController(*game, mainInterface, scene);
    sut.setHoveredObject(creature.get());
    sut.setSelectedObject(creature.get());

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEBUTTONDOWN;
    evt.button.x = 1;
    evt.button.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_TEST(handled);
    BOOST_TEST(0ll == game->changeCursorInvocations().size());
    BOOST_TEST(1ll == creature->handleClickInvocations().size());
    BOOST_TEST(creature.get() == sut.hoveredObject());
    BOOST_TEST(creature.get() == sut.selectedObject());
}

BOOST_AUTO_TEST_CASE(should_reset_selected_target_on_mouse_click_when_no_object_at_mouse_position) {
    // given
    auto test = FunctionalTest();
    auto game = test.mockGame();
    auto creature = game->mockCreature();
    auto mainInterface = MockMainInterface();
    auto &scene = test.sceneMockByName(kSceneMain);

    auto sut = SelectionController(*game, mainInterface, scene);
    sut.setSelectedObject(creature.get());

    auto evt = SDL_Event();
    evt.type = SDL_MOUSEBUTTONDOWN;
    evt.button.x = 1;
    evt.button.y = 2;

    // when
    auto handled = sut.handle(evt);

    // then
    BOOST_TEST(handled);
    BOOST_TEST(0ll == game->changeCursorInvocations().size());
    BOOST_TEST(creature->handleClickInvocations().empty());
    BOOST_TEST(nullptr == sut.hoveredObject());
    BOOST_TEST(nullptr == sut.selectedObject());
}

BOOST_AUTO_TEST_SUITE_END()
