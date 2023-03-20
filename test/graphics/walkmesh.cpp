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

#include "../../src/graphics/walkmesh.h"

using namespace std;

using namespace reone;
using namespace reone::graphics;

BOOST_AUTO_TEST_SUITE(walkmesh)

BOOST_AUTO_TEST_CASE(should_find_ray_walkmesh_intersection__intersection_from_close) {
    // given
    auto walkmesh = Walkmesh();
    walkmesh.add(Walkmesh::Face {0, 0, vector<glm::vec3> {glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {1, 0, vector<glm::vec3> {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {2, 0, vector<glm::vec3> {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {3, 0, vector<glm::vec3> {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    auto rootAabb = make_shared<Walkmesh::AABB>();
    rootAabb->value = AABB(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    rootAabb->left = make_shared<Walkmesh::AABB>();
    rootAabb->left->value = AABB(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rootAabb->left->left = make_shared<Walkmesh::AABB>();
    rootAabb->left->left->faceIdx = 0;
    rootAabb->left->right = make_shared<Walkmesh::AABB>();
    rootAabb->left->right->faceIdx = 1;
    rootAabb->right = make_shared<Walkmesh::AABB>();
    rootAabb->right->value = AABB(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rootAabb->right->left = make_shared<Walkmesh::AABB>();
    rootAabb->right->left->faceIdx = 2;
    rootAabb->right->right = make_shared<Walkmesh::AABB>();
    rootAabb->right->right->faceIdx = 3;
    walkmesh.setRootAABB(rootAabb);

    // when
    float distance = -1.0f;
    auto face = walkmesh.raycast(set<uint32_t> {0}, glm::vec3(-0.5f, 0.25, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), 10.0f, distance);

    // then
    BOOST_REQUIRE(static_cast<bool>(face));
    BOOST_TEST(0 == face->index);
    BOOST_CHECK_CLOSE(1.0f, distance, 1e-5);
}

BOOST_AUTO_TEST_CASE(should_find_ray_walkmesh_intersection__intersection_from_far) {
    // given
    auto walkmesh = Walkmesh();
    walkmesh.add(Walkmesh::Face {0, 0, vector<glm::vec3> {glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {1, 0, vector<glm::vec3> {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {2, 0, vector<glm::vec3> {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {3, 0, vector<glm::vec3> {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    auto rootAabb = make_shared<Walkmesh::AABB>();
    rootAabb->value = AABB(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    rootAabb->left = make_shared<Walkmesh::AABB>();
    rootAabb->left->value = AABB(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rootAabb->left->left = make_shared<Walkmesh::AABB>();
    rootAabb->left->left->faceIdx = 0;
    rootAabb->left->right = make_shared<Walkmesh::AABB>();
    rootAabb->left->right->faceIdx = 1;
    rootAabb->right = make_shared<Walkmesh::AABB>();
    rootAabb->right->value = AABB(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rootAabb->right->left = make_shared<Walkmesh::AABB>();
    rootAabb->right->left->faceIdx = 2;
    rootAabb->right->right = make_shared<Walkmesh::AABB>();
    rootAabb->right->right->faceIdx = 3;
    walkmesh.setRootAABB(rootAabb);

    // when
    float distance = -1.0f;
    auto face = walkmesh.raycast(set<uint32_t> {0}, glm::vec3(-0.5f, 0.25, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), 10.0f, distance);

    // then
    BOOST_REQUIRE(!static_cast<bool>(face));
}

BOOST_AUTO_TEST_CASE(should_find_ray_walkmesh_intersection__no_intersection) {
    // given
    auto walkmesh = Walkmesh();
    walkmesh.add(Walkmesh::Face {0, 0, vector<glm::vec3> {glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {1, 0, vector<glm::vec3> {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {2, 0, vector<glm::vec3> {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    walkmesh.add(Walkmesh::Face {3, 0, vector<glm::vec3> {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)}, glm::vec3(1.0f, 0.0f, 0.0f)});
    auto rootAabb = make_shared<Walkmesh::AABB>();
    rootAabb->value = AABB(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    rootAabb->left = make_shared<Walkmesh::AABB>();
    rootAabb->left->value = AABB(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rootAabb->left->left = make_shared<Walkmesh::AABB>();
    rootAabb->left->left->faceIdx = 0;
    rootAabb->left->right = make_shared<Walkmesh::AABB>();
    rootAabb->left->right->faceIdx = 1;
    rootAabb->right = make_shared<Walkmesh::AABB>();
    rootAabb->right->value = AABB(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rootAabb->right->left = make_shared<Walkmesh::AABB>();
    rootAabb->right->left->faceIdx = 2;
    rootAabb->right->right = make_shared<Walkmesh::AABB>();
    rootAabb->right->right->faceIdx = 3;
    walkmesh.setRootAABB(rootAabb);

    // when
    float distance = -1.0f;
    auto face = walkmesh.raycast(set<uint32_t> {0}, glm::vec3(-0.5f, 0.25, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, distance);

    // then
    BOOST_REQUIRE(!static_cast<bool>(face));
}

BOOST_AUTO_TEST_SUITE_END()
