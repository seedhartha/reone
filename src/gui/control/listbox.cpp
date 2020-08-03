/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "listbox.h"

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../../core/log.h"
#include "../../render/mesh/guiquad.h"
#include "../../render/shadermanager.h"
#include "../../resources/manager.h"

#include "button.h"

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace gui {

static const int kItemPadding = 3;

ListBox::ListBox() : Control(ControlType::ListBox) {
}

ListBox::ListBox(const std::string &tag) : Control(ControlType::ListBox, tag) {
}

void ListBox::loadCustom() {
    ResourceManager &resources = ResourceManager::instance();

    Control::Border border;
    border.fill = resources.findTexture("brightblue", TextureType::Diffuse);

    std::unique_ptr<Control> protoItem(new Button());
    protoItem->setExtent(Control::Extent(0, 0, _extent.width, _text.font->height() + 2 * kItemPadding));
    protoItem->setBorder(border);
    _protoItem = std::move(protoItem);

    _padding = 3;
    _slotCount = _extent.height / (_protoItem->extent().height + _padding);
}

void ListBox::add(const Item &item) {
    _items.push_back(item);
}

bool ListBox::handleMouseWheel(int x, int y) {
    if (y < 0) {
        if (_itemOffset < _items.size() - _slotCount) _itemOffset++;
        return true;
    } else if (y > 0) {
        if (_itemOffset > 0) _itemOffset--;
        return true;
    }

    return false;
}

bool ListBox::handleClick(int x, int y) {
    int itemIdx = (y - _extent.top) / (_protoItem->extent().height + _padding) + _itemOffset;
    if (itemIdx >= _items.size()) return false;

    if (_onItemClicked) {
        _onItemClicked(_tag, _items[itemIdx].tag);
        return true;
    }

    return false;
}

void ListBox::initGL() {
    Control::initGL();

    if (_protoItem) _protoItem->initGL();
}

void ListBox::render(const glm::mat4 &transform) const {
    if (!_protoItem) return;

    ShaderManager &shaders = ShaderManager::instance();
    shaders.activate(ShaderProgram::BasicDiffuse);
    shaders.setUniform("alpha", 1.0f);
    shaders.setUniform("color", glm::vec3(1.0f));

    std::shared_ptr<Texture> itemFill(_protoItem->border().fill);
    assert(itemFill);

    glActiveTexture(0);
    itemFill->bind();

    int x = _extent.left;
    int y = _extent.top;

    glm::mat4 transform2(1.0f);
    GUIQuad &quad = GUIQuad::instance();

    for (int i = 0; i < _items.size() && i < _slotCount; ++i) {
        transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
        transform2 = glm::scale(transform2, glm::vec3(_extent.width, _protoItem->extent().height, 1.0f));

        shaders.setUniform("model", transform2);
        quad.render(GL_TRIANGLES);

        y += _protoItem->extent().height + _padding;
    }

    itemFill->unbind();
    shaders.deactivate();

    x = _extent.left + 0.5f * _extent.width;
    y = _extent.top + 0.5f * _protoItem->extent().height;

    for (int i = 0; i < _items.size() && i < _slotCount; ++i) {
        int itemIdx = i + _itemOffset;
        transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

        _text.font->render(_items[itemIdx].text, transform2);

        y += _protoItem->extent().height + _padding;
    }
}

} // namespace gui

} // namespace reone
