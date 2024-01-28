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

#include <gtest/gtest.h>

#include "../../../fixtures/audio.h"
#include "../../../fixtures/graphics.h"
#include "../../../fixtures/resource.h"
#include "../../../fixtures/scene.h"

#include "reone/game/neo/controller/playercamera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/input/event.h"

using namespace reone;
using namespace reone::audio;
using namespace reone::game::neo;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

using testing::_;
using testing::Return;
using testing::Test;

class PlayerCameraControllerFixture : public Test {
protected:
    void SetUp() override {
        _graphicsModule.init();
        _audioModule.init();
        _resourceModule.init();

        _camera = std::make_shared<CameraSceneNode>(
            _sceneGraph,
            _graphicsModule.services(),
            _audioModule.services(),
            _resourceModule.services());
        _camera->setPerspectiveProjection(glm::radians(55.0f), 1.0f, 0.25f, 2500.0f);
        auto model = std::make_shared<Model>(
            "",
            0,
            std::shared_ptr<ModelNode>(),
            std::vector<std::shared_ptr<Animation>>(),
            "",
            1.0f);
        _playerSceneNode = std::make_shared<ModelSceneNode>(
            *model,
            ModelUsage::Creature,
            _sceneGraph,
            _graphicsModule.services(),
            _audioModule.services(),
            _resourceModule.services());
        _subject.setCamera(*_camera);
        _subject.setGameLogicExecutor([](AsyncTask task) {
            task();
        });
    }

    TestGraphicsModule _graphicsModule;
    TestAudioModule _audioModule;
    TestResourceModule _resourceModule;
    MockSceneGraph _sceneGraph;

    std::shared_ptr<CameraSceneNode> _camera;
    std::shared_ptr<ModelSceneNode> _playerSceneNode;
    Creature _player {0, ""};

    PlayerCameraController _subject {_sceneGraph};
};

TEST_F(PlayerCameraControllerFixture, should_move_and_rotate_camera_given_no_player) {
    // expect
    glm::vec3 scale, translation, skew;
    glm::quat orientation, expectedOrientation;
    glm::vec4 perspective;

    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, 0.0f, 1e-4f);
    expectedOrientation = glm::quat_cast(glm::eulerAngleZX(0.0f, 0.0f));
    EXPECT_NEAR(orientation.x, expectedOrientation.x, 1e-4f);
    EXPECT_NEAR(orientation.y, expectedOrientation.y, 1e-4f);
    EXPECT_NEAR(orientation.z, expectedOrientation.z, 1e-4f);
    EXPECT_NEAR(orientation.w, expectedOrientation.w, 1e-4f);

    auto wKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::W, 0, false});
    EXPECT_TRUE(_subject.handle(wKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 8.0f, 1e-4f);
    EXPECT_NEAR(translation.z, -8.0f, 1e-4f);

    auto wKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::W, 0, false});
    EXPECT_TRUE(_subject.handle(wKeyUpEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);

    auto sKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::S, 0, false});
    EXPECT_TRUE(_subject.handle(sKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, 0.0f, 1e-4f);

    auto sKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::S, 0, false});
    EXPECT_TRUE(_subject.handle(sKeyUpEvent));

    auto aKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::A, 0, false});
    EXPECT_TRUE(_subject.handle(aKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, -8.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, 0.0f, 1e-4f);

    auto aKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::A, 0, false});
    EXPECT_TRUE(_subject.handle(aKeyUpEvent));

    auto dKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::D, 0, false});
    EXPECT_TRUE(_subject.handle(dKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, 0.0f, 1e-4f);

    auto dKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::D, 0, false});
    EXPECT_TRUE(_subject.handle(dKeyUpEvent));

    auto qKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::Q, 0, false});
    EXPECT_TRUE(_subject.handle(qKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, 8.0f, 1e-4f);

    auto qKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::Q, 0, false});
    EXPECT_TRUE(_subject.handle(qKeyUpEvent));

    auto zKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::Z, 0, false});
    EXPECT_TRUE(_subject.handle(zKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, 0.0f, 1e-4f);

    auto zKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::Z, 0, false});
    EXPECT_TRUE(_subject.handle(zKeyUpEvent));

    auto xMotionEvent = input::Event::newMouseMotion({101, 100, 1, 0});
    EXPECT_TRUE(_subject.handle(xMotionEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    expectedOrientation = glm::quat_cast(glm::eulerAngleZX(-0.001f, 0.0f));
    EXPECT_NEAR(orientation.x, expectedOrientation.x, 1e-4f);
    EXPECT_NEAR(orientation.y, expectedOrientation.y, 1e-4f);
    EXPECT_NEAR(orientation.z, expectedOrientation.z, 1e-4f);
    EXPECT_NEAR(orientation.w, expectedOrientation.w, 1e-4f);

    auto yMotionEvent = input::Event::newMouseMotion({101, 99, 0, -1});
    EXPECT_TRUE(_subject.handle(yMotionEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    expectedOrientation = glm::quat_cast(glm::eulerAngleZX(-0.001f, 0.001f));
    EXPECT_NEAR(orientation.x, expectedOrientation.x, 1e-4f);
    EXPECT_NEAR(orientation.y, expectedOrientation.y, 1e-4f);
    EXPECT_NEAR(orientation.z, expectedOrientation.z, 1e-4f);
    EXPECT_NEAR(orientation.w, expectedOrientation.w, 1e-4f);
}

TEST_F(PlayerCameraControllerFixture, should_rotate_camera_around_player_and_move_player) {
    // given
    _subject.setPlayer(_player);
    _subject.setPlayerSceneNode(*_playerSceneNode);

    // expect
    glm::vec3 scale, translation, skew;
    glm::quat orientation, expectedOrientation;
    glm::vec4 perspective;

    EXPECT_CALL(_sceneGraph, testLineOfSight(_, _, _)).WillRepeatedly(Return(false));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, -1.2f, 1e-4f);
    expectedOrientation = glm::quat_cast(glm::eulerAngleZX(0.0f, 0.0f));
    EXPECT_NEAR(orientation.x, expectedOrientation.x, 1e-4f);
    EXPECT_NEAR(orientation.y, expectedOrientation.y, 1e-4f);
    EXPECT_NEAR(orientation.z, expectedOrientation.z, 1e-4f);
    EXPECT_NEAR(orientation.w, expectedOrientation.w, 1e-4f);
    EXPECT_FALSE(_player.currentAction().has_value());

    auto wKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::W, 0, false});
    EXPECT_TRUE(_subject.handle(wKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, -1.2f, 1e-4f);
    EXPECT_TRUE(_player.currentAction().has_value());

    auto wKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::W, 0, false});
    EXPECT_TRUE(_subject.handle(wKeyUpEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);

    auto sKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::S, 0, false});
    EXPECT_TRUE(_subject.handle(sKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, -1.2f, 1e-4f);

    auto sKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::S, 0, false});
    EXPECT_TRUE(_subject.handle(sKeyUpEvent));

    auto aKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::A, 0, false});
    EXPECT_TRUE(_subject.handle(aKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, -1.2f, 1e-4f);

    auto aKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::A, 0, false});
    EXPECT_TRUE(_subject.handle(aKeyUpEvent));

    auto dKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::D, 0, false});
    EXPECT_TRUE(_subject.handle(dKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, -1.2f, 1e-4f);

    auto dKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::D, 0, false});
    EXPECT_TRUE(_subject.handle(dKeyUpEvent));

    auto zKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::Z, 0, false});
    EXPECT_TRUE(_subject.handle(zKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, -1.2f, 1e-4f);

    auto zKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::Z, 0, false});
    EXPECT_TRUE(_subject.handle(zKeyUpEvent));

    auto cKeyDownEvent = input::Event::newKeyDown({true, input::KeyCode::C, 0, false});
    EXPECT_TRUE(_subject.handle(cKeyDownEvent));
    _subject.update(1.0f);
    glm::decompose(_camera->localTransform(), scale, orientation, translation, skew, perspective);
    EXPECT_NEAR(translation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(translation.z, -1.2f, 1e-4f);

    auto cKeyUpEvent = input::Event::newKeyUp({false, input::KeyCode::C, 0, false});
    EXPECT_TRUE(_subject.handle(cKeyUpEvent));
}
