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

#include "reone/system/stream/bytearrayoutput.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

class StubProvider : public IResourceProvider {
public:
    void add(ResourceId id, shared_ptr<ByteArray> res) {
        _resources.insert(make_pair(id, move(res)));
    }

    shared_ptr<ByteArray> find(const ResourceId &id) override { return _resources.at(id); }

    int id() const override { return 0; };

private:
    unordered_map<ResourceId, shared_ptr<ByteArray>, ResourceIdHasher> _resources;
};

BOOST_AUTO_TEST_SUITE(gffs)

BOOST_AUTO_TEST_CASE(should_get_gff_with_caching) {
    // given

    auto resBytes = make_shared<ByteArray>();
    auto res = ByteArrayOutputStream(*resBytes);
    res.write("GFF V3.2");
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

    auto provider = make_unique<StubProvider>();
    provider->add(ResourceId("sample", ResourceType::Gff), resBytes);

    auto resources = Resources();
    resources.indexProvider(move(provider), "[stub]", false);

    auto gffs = Gffs(resources);

    // when

    auto gff1 = gffs.get("sample", ResourceType::Gff);

    resources.clearAllProviders();

    auto gff2 = gffs.get("sample", ResourceType::Gff);

    // then

    BOOST_TEST(static_cast<bool>(gff1));
    BOOST_TEST(static_cast<bool>(gff2));
    BOOST_TEST(gff1.get() == gff2.get());
}

BOOST_AUTO_TEST_SUITE_END()
