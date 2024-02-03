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

#include "reone/graphics/format/mdlmdxreader.h"
#include "reone/graphics/model.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

#include "../../fixtures/graphics.h"

using namespace reone;
using namespace reone::graphics;

TEST(MdlReader, should_load_mdl) {
    // given
    auto mdlBytes = StringBuilder()
                        // File Header (0)
                        .append("\x00\x00\x00\x00", 4) // unknown
                        .append("\x2e\x01\x00\x00", 4) // MDL size
                        .append("\x00\x00\x00\x00", 4) // MDX size
                        // Geometry Header (12)
                        .append("\x00\x00\x00\x00", 4)                                                                                    // model function pointer 1
                        .append("\x00\x00\x00\x00", 4)                                                                                    // model function pointer 2
                        .append("some_model\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32) // model name
                        .append("\xd2\x00\x00\x00", 4)                                                                                    // offset to root node
                        .append("\x01\x00\x00\x00", 4)                                                                                    // number of nodes
                        .append('\x00', 6 * 4)                                                                                            // unknown
                        .append("\x00\x00\x00\x00", 4)                                                                                    // reference counter
                        .append("\x00\x00\x00\x00", 4)                                                                                    // model type
                        // Model Header (92)
                        .append("\x00", 1)                                                                                                                  // classification
                        .append("\x00", 1)                                                                                                                  // subclassification
                        .append("\x00", 1)                                                                                                                  // unknown
                        .append("\x00", 1)                                                                                                                  // affected by fog
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // number of child models
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // offset to animations
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // number of animations
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // number of animations
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // supermodel reference
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // bounding box (X min)
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // bounding box (Y min)
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // bounding box (Z min)
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // bounding box (X max)
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // bounding box (Y max)
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // bounding box (Z max)
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // radius
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // animation scale
                        .append("NULL\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32) // supermodel name
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // offset to animation root node
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // unknown
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // MDX size
                        .append("\x00\x00\x00\x00", 4)                                                                                                      // offset to MDX
                        .append("\xc4\x00\x00\x00", 4)                                                                                                      // offset to names
                        .append("\x01\x00\x00\x00", 4)                                                                                                      // number of names
                        .append("\x01\x00\x00\x00", 4)                                                                                                      // number of names
                        // Name Offsets (208)
                        .append("\xc8\x00\x00\x00", 4)
                        // Names (212)
                        .append("root_node\x00", 10)
                        // Root Node (222)
                        .append("\x00\x00", 2)         // flags
                        .append("\x00\x00", 2)         // node number
                        .append("\x00\x00", 2)         // name index
                        .append("\x00\x00", 2)         // padding
                        .append("\x00\x00\x00\x00", 4) // offset to root node
                        .append("\x00\x00\x00\x00", 4) // offset to parent node
                        .append("\x00\x00\x00\x00", 4) // position (X)
                        .append("\x00\x00\x00\x00", 4) // position (Y)
                        .append("\x00\x00\x00\x00", 4) // position (Z)
                        .append("\x00\x00\x00\x00", 4) // orientation (W)
                        .append("\x00\x00\x00\x00", 4) // orientation (X)
                        .append("\x00\x00\x00\x00", 4) // orientation (Y)
                        .append("\x00\x00\x00\x00", 4) // orientation (Z)
                        .append("\x00\x00\x00\x00", 4) // offset to children
                        .append("\x00\x00\x00\x00", 4) // number of children
                        .append("\x00\x00\x00\x00", 4) // number of children
                        .append("\x00\x00\x00\x00", 4) // offset to controller keys
                        .append("\x00\x00\x00\x00", 4) // number of controller keys
                        .append("\x00\x00\x00\x00", 4) // number of controller keys
                        .append("\x00\x00\x00\x00", 4) // offset to controller values
                        .append("\x00\x00\x00\x00", 4) // number of controller values
                        .append("\x00\x00\x00\x00", 4) // number of controller values
                        .string();
    auto mdl = MemoryInputStream(mdlBytes);
    auto mdxBytes = StringBuilder().string();
    auto mdx = MemoryInputStream(mdxBytes);
    Statistic statistic;
    auto reader = MdlMdxReader(mdl, mdx, statistic);

    // when
    reader.load();

    // then
    auto model = reader.model();
    EXPECT_TRUE(static_cast<bool>(model));
    EXPECT_EQ(std::string("some_model"), model->name());
    auto rootNode = model->rootNode();
    EXPECT_EQ(std::string("root_node"), rootNode->name());
}
