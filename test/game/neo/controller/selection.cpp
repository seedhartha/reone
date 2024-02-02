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

#include "reone/game/neo/controller/selection.h"
#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/module.h"
#include "reone/graphics/options.h"
#include "reone/input/event.h"
#include "reone/scene/collision.h"

using namespace reone;
using namespace reone::audio;
using namespace reone::game::neo;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

using testing::_;
using testing::Return;
using testing::Test;

class SelectionControllerFixture : public Test {
protected:
    void SetUp() override {
        _graphicsOpt.width = 1000;
        _graphicsOpt.height = 1000;

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
        _creatureSceneNode = std::make_shared<ModelSceneNode>(
            *model,
            ModelUsage::Creature,
            _sceneGraph,
            _graphicsModule.services(),
            _audioModule.services(),
            _resourceModule.services());
        _creatureSceneNode->setExternalId(reinterpret_cast<void *>(_creature.id()));

        _area.add(_creature);
        _module.setArea(_area);

        _subject.setCamera(*_camera);
        _subject.setModule(_module);
    }

    GraphicsOptions _graphicsOpt;
    TestGraphicsModule _graphicsModule;
    TestAudioModule _audioModule;
    TestResourceModule _resourceModule;
    MockSceneGraph _sceneGraph;

    Module _module {0, ""};
    Area _area {1, ""};
    Creature _creature {2, ""};

    std::shared_ptr<CameraSceneNode> _camera;
    std::shared_ptr<ModelSceneNode> _creatureSceneNode;

    SelectionController _subject {_graphicsOpt, _sceneGraph};
};

TEST_F(SelectionControllerFixture, should_pick_hovered_object_on_motion_and_select_object_on_click) {
    // expect
    std::optional<std::reference_wrapper<Object>> hovered;
    std::optional<std::reference_wrapper<Object>> selected;

    EXPECT_CALL(_sceneGraph, pickModelRay(_, _)).WillOnce(Return(std::ref(*_creatureSceneNode)));
    auto motionEvent = input::Event::newMouseMotion({500, 500, 0, 0});
    _subject.handle(motionEvent);

    hovered = _subject.hoveredObject();
    EXPECT_TRUE(hovered.has_value());
    EXPECT_EQ(hovered->get(), _creature);

    selected = _subject.selectedObject();
    EXPECT_FALSE(selected.has_value());

    auto buttonDownEvent = input::Event::newMouseButtonDown({input::MouseButton::Left, true, 1, 500, 500});
    _subject.handle(buttonDownEvent);

    hovered = _subject.hoveredObject();
    EXPECT_TRUE(hovered.has_value());
    EXPECT_EQ(hovered->get(), _creature);

    selected = _subject.selectedObject();
    EXPECT_TRUE(selected.has_value());
    EXPECT_EQ(selected->get(), _creature);
}
