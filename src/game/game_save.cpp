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
#include "../resource/format/erfreader.h"
#include "../resource/format/erfwriter.h"
#include "../resource/format/gffwriter.h"

namespace fs = boost::filesystem;
namespace io = boost::iostreams;

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kNfoBufferSize = 1024;

void Game::saveToFile(const fs::path &path) {
    GffStruct::Field nfoLastModuleFld(GffStruct::FieldType::CExoString, "LastModule");
    nfoLastModuleFld.strValue = _module->name();

    auto nfoRoot = make_shared<GffStruct>(0xffffffff);
    nfoRoot->add(move(nfoLastModuleFld));

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

    ErfWriter erf;
    erf.add(move(nfoRes));
    erf.save(ErfWriter::FileType::ERF, path);
}

void Game::loadFromFile(const fs::path &path) {
    ErfReader erf;
    erf.load(path);

    shared_ptr<ByteArray> nfoData(erf.find("savenfo", ResourceType::Res));

    GffReader nfo;
    nfo.load(wrap(nfoData));

    shared_ptr<GffStruct> nfoRoot(nfo.root());

    string lastModule(nfoRoot->getString("LastModule"));
    scheduleModuleTransition(lastModule, "");
}

} // namespace game

} // namespace reone
