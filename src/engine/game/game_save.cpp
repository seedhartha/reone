/*
 * Copyright (c) 2020-2021 The reone project contributors
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

/** @file
 *  Game routines related to saved games.
 */

#include "game.h"

#include <boost/iostreams/stream.hpp>

#include "../common/streamutil.h"
#include "../graphics/texture/tgawriter.h"
#include "../resource/format/erfreader.h"
#include "../resource/format/erfwriter.h"
#include "../resource/format/gffwriter.h"

#include "enginetype/location.h"
#include "party.h"

namespace fs = boost::filesystem;
namespace io = boost::iostreams;

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kNfoBufferSize = 1024;
static constexpr int kScreenBufferSize = 262144;

void Game::saveToFile(const fs::path &path) {
    // Prepare savenfo RES

    vector<shared_ptr<GffStruct>> nfoPartyMembers;
    for (int i = 0; i < 3; ++i) {
        if (_game->party().getSize() > i) {
            nfoPartyMembers.push_back(getPartyMemberNFOStruct(i));
        }
    }

    vector<shared_ptr<GffStruct>> nfoGlobalBooleans;
    for (auto &global : _globalBooleans) {
        auto gffs = make_shared<GffStruct>(2, vector<GffStruct::Field> {
            GffStruct::Field::newCExoString("Name", global.first),
            GffStruct::Field::newByte("Value", static_cast<uint32_t>(global.second))
        });
        nfoGlobalBooleans.push_back(move(gffs));
    }

    vector<shared_ptr<GffStruct>> nfoGlobalNumbers;
    for (auto &global : _globalNumbers) {
        auto gffs = make_shared<GffStruct>(2, vector<GffStruct::Field> {
            GffStruct::Field::newCExoString("Name", global.first),
            GffStruct::Field::newInt("Value", global.second)
        });
        nfoGlobalNumbers.push_back(move(gffs));
    }

    vector<shared_ptr<GffStruct>> nfoGlobalStrings;
    for (auto &global : _globalStrings) {
        auto gffs = make_shared<GffStruct>(2, vector<GffStruct::Field> {
            GffStruct::Field::newCExoString("Name", global.first),
            GffStruct::Field::newCExoString("Value", global.second)
        });
        nfoGlobalStrings.push_back(move(gffs));
    }

    vector<shared_ptr<GffStruct>> nfoGlobalLocations;
    for (auto &global : _globalLocations) {
        auto gffs = make_shared<GffStruct>(3, vector<GffStruct::Field> {
            GffStruct::Field::newCExoString("Name", global.first),
            GffStruct::Field::newVector("Position", global.second->position()),
            GffStruct::Field::newFloat("Facing", global.second->facing())
        });
        nfoGlobalLocations.push_back(move(gffs));
    }

    auto nfoRoot = make_shared<GffStruct>(0xffffffff);
    nfoRoot->add(GffStruct::Field::newCExoString("LastModule", _module->name()));
    nfoRoot->add(GffStruct::Field::newList("Party", move(nfoPartyMembers)));
    nfoRoot->add(GffStruct::Field::newList("GlobalBooleans", move(nfoGlobalBooleans)));
    nfoRoot->add(GffStruct::Field::newList("GlobalNumbers", move(nfoGlobalNumbers)));
    nfoRoot->add(GffStruct::Field::newList("GlobalStrings", move(nfoGlobalStrings)));
    nfoRoot->add(GffStruct::Field::newList("GlobalLocations", move(nfoGlobalLocations)));

    char nfoBuffer[kNfoBufferSize];
    io::array_sink nfoSink(nfoBuffer, kNfoBufferSize);
    auto nfoStream = make_shared<io::stream<io::array_sink>>(nfoSink);

    GffWriter nfo(ResourceType::Res, nfoRoot);
    nfo.save(nfoStream);
    size_t nfoSize = nfoStream->tellp();

    ByteArray nfoResData;
    nfoResData.resize(nfoSize);
    memcpy(&nfoResData[0], nfoBuffer, nfoSize);

    ErfWriter::Resource nfoRes;
    nfoRes.resRef = "savenfo";
    nfoRes.resType = ResourceType::Res;
    nfoRes.data = move(nfoResData);

    // Prepare screen TGA

    ByteArray screenBuffer;
    screenBuffer.resize(kScreenBufferSize);
    io::array_sink screenSink(&screenBuffer[0], kScreenBufferSize);
    auto screenStream = make_shared<io::stream<io::array_sink>>(screenSink);

    shared_ptr<Texture> screenshot(_scene.worldRenderPipeline().screenshot());
    TgaWriter tga(screenshot);
    tga.save(*screenStream);
    screenBuffer.resize(screenStream->tellp());

    ErfWriter::Resource screenRes;
    screenRes.resRef = "screen";
    screenRes.resType = ResourceType::Tga;
    screenRes.data = move(screenBuffer);

    // Save ERF

    ErfWriter erf;
    erf.add(move(nfoRes));
    erf.add(move(screenRes));
    erf.save(ErfWriter::FileType::ERF, path);
}

shared_ptr<GffStruct> Game::getPartyMemberNFOStruct(int index) const {
    auto member = _game->party().getMember(index);

    return make_shared<GffStruct>(0, vector<GffStruct::Field> {
        GffStruct::Field::newByte("NPC", _game->party().getNPCByMemberIndex(index)),
        GffStruct::Field::newCExoString("TemplateResRef", member->blueprintResRef()),
        GffStruct::Field::newVector("Position", member->position()),
        GffStruct::Field::newFloat("Facing", member->getFacing())
    });
}

void Game::loadFromFile(const fs::path &path) {
    setLoadFromSaveGame(true);

    ErfReader erf;
    erf.load(path);

    shared_ptr<ByteArray> nfoData(erf.find("savenfo", ResourceType::Res));

    GffReader nfo;
    nfo.load(wrap(nfoData));

    shared_ptr<GffStruct> nfoRoot(nfo.root());

    // Module
    string lastModule(nfoRoot->getString("LastModule"));
    scheduleModuleTransition(lastModule, "");

    // Party
    vector<shared_ptr<GffStruct>> nfoParty(nfoRoot->getList("Party"));
    _game->party().clear();
    for (size_t i = 0; i < nfoParty.size(); ++i) {
        shared_ptr<GffStruct> member(nfoParty[i]);
        int npc = member->getInt("NPC");
        string blueprintResRef(member->getString("TemplateResRef"));
        glm::vec3 position(member->getVector("Position"));
        float facing = member->getFloat("Facing");

        shared_ptr<Creature> creature(_game->objectFactory().newCreature());
        if (npc == -1) {
            creature->setTag(kObjectTagPlayer);
        }
        creature->loadFromBlueprint(blueprintResRef);
        creature->setImmortal(true);
        creature->setPosition(move(position));
        creature->setFacing(facing);
        _game->party().addMember(npc, creature);
        if (i == 0) {
            _game->party().setPlayer(creature);
        }
    }

    // Globals
    for (auto &global : nfoRoot->getList("GlobalBooleans")) {
        setGlobalBoolean(global->getString("Name"), global->getBool("Value"));
    }
    for (auto &global : nfoRoot->getList("GlobalNumbers")) {
        setGlobalNumber(global->getString("Name"), global->getInt("Value"));
    }
    for (auto &global : nfoRoot->getList("GlobalStrings")) {
        setGlobalString(global->getString("Name"), global->getString("Value"));
    }
    for (auto &global : nfoRoot->getList("GlobalLocations")) {
        setGlobalLocation(global->getString("Name"), make_shared<Location>(global->getVector("Position"), global->getFloat("Facing")));
    }
}

} // namespace game

} // namespace reone
