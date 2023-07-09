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

#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryoutput.h"

using namespace reone;
using namespace reone::resource;

class StubProvider : public IResourceProvider {
public:
    void add(ResourceId id, std::shared_ptr<ByteArray> res) {
        _resources.insert(std::make_pair(id, std::move(res)));
    }

    std::shared_ptr<ByteArray> find(const ResourceId &id) override { return _resources.at(id); }

    int id() const override { return 0; };

private:
    std::unordered_map<ResourceId, std::shared_ptr<ByteArray>, ResourceIdHasher> _resources;
};

TEST(gffs, should_get_gff_with_caching) {
    // given

    auto resBytes = std::make_shared<ByteArray>();
    auto res = MemoryOutputStream(*resBytes);
    res.write("GFF V3.2", 8);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);

    auto provider = std::make_unique<StubProvider>();
    provider->add(ResourceId("sample", ResourceType::Gff), resBytes);

    auto resources = Resources();
    resources.indexProvider(std::move(provider), "[stub]", false);

    auto gffs = Gffs(resources);

    // when

    auto gff1 = gffs.get("sample", ResourceType::Gff);

    resources.clearAllProviders();

    auto gff2 = gffs.get("sample", ResourceType::Gff);

    // then

    EXPECT_TRUE(static_cast<bool>(gff1));
    EXPECT_TRUE(static_cast<bool>(gff2));
    EXPECT_EQ(gff1.get(), gff2.get());
}
