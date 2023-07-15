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

#include "reone/game/cursors.h"

#include "reone/graphics/cursor.h"
#include "reone/graphics/format/curreader.h"
#include "reone/graphics/texture.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static std::unordered_map<CursorType, std::pair<uint32_t, uint32_t>> g_groupNamesByType {
    {CursorType::Default, {1, 2}},
    {CursorType::Talk, {11, 12}},
    {CursorType::Door, {23, 24}},
    {CursorType::Pickup, {25, 26}},
    {CursorType::DisableMine, {33, 34}},
    {CursorType::RecoverMine, {37, 38}},
    {CursorType::Attack, {51, 52}}};

void Cursors::deinit() {
    _cache.clear();
}

std::shared_ptr<Cursor> Cursors::get(CursorType type) {
    auto maybeCursor = _cache.find(type);
    if (maybeCursor != _cache.end()) {
        return maybeCursor->second;
    }
    const std::pair<uint32_t, uint32_t> &groupNames = getCursorGroupNames(type);
    std::vector<uint32_t> cursorNamesUp(getCursorNamesFromCursorGroup(groupNames.first));
    if (cursorNamesUp.empty()) {
        return nullptr;
    }
    std::vector<uint32_t> cursorNamesDown(getCursorNamesFromCursorGroup(groupNames.second));
    if (cursorNamesDown.empty()) {
        return nullptr;
    }
    std::shared_ptr<Texture> textureUp(newTextureFromCursor(cursorNamesUp.back()));
    textureUp->init();
    std::shared_ptr<Texture> textureDown(newTextureFromCursor(cursorNamesDown.back()));
    textureDown->init();

    auto cursor = std::make_shared<Cursor>(textureUp, textureDown, _graphicsContext, _meshes, _shaders, _textures, _uniforms, _window);
    _cache.insert(std::make_pair(type, cursor));

    return std::move(cursor);
}

const std::pair<uint32_t, uint32_t> &Cursors::getCursorGroupNames(CursorType type) {
    auto maybeGroupNames = g_groupNamesByType.find(type);
    if (maybeGroupNames == g_groupNamesByType.end()) {
        throw ResourceNotFoundException("Cursor group not found: " + std::to_string(static_cast<int>(type)));
    }
    return maybeGroupNames->second;
}

std::vector<uint32_t> Cursors::getCursorNamesFromCursorGroup(uint32_t name) {
    auto res = _resources.find(ResourceId(std::to_string(name), ResourceType::CursorGroup));
    if (!res) {
        return std::vector<uint32_t>();
    }
    auto stream = MemoryInputStream(res->data);
    auto reader = BinaryReader(stream);

    reader.skipBytes(4); // Reserved, ResType
    uint16_t resCount = reader.readUint16();

    std::vector<uint32_t> cursorNames;
    for (uint16_t i = 0; i < resCount; ++i) {
        reader.skipBytes(12); // Cursor, Planes, BitCount, BytesInRes
        uint16_t cursorId = reader.readUint16();
        cursorNames.push_back(static_cast<uint32_t>(cursorId));
    }

    return cursorNames;
}

std::shared_ptr<Texture> Cursors::newTextureFromCursor(uint32_t name) {
    auto [data, _] = _resources.get(ResourceId(std::to_string(name), ResourceType::Cursor));
    auto stream = MemoryInputStream(data);

    CurReader cur(stream);
    cur.load();

    return cur.texture();
}

} // namespace game

} // namespace reone
