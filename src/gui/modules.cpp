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

#include "modules.h"

#include "../resources/manager.h"

#include "control/label.h"
#include "control/listbox.h"

using namespace std::placeholders;

using namespace reone::resources;

namespace reone {

namespace gui {

static const int kMargin = 10;
static const int kWidth = 400;

ModulesGui::ModulesGui(const render::GraphicsOptions &opts) : GUI(opts) {
}

void ModulesGui::load() {
    loadFont();
    assert(_font);

    loadLabel();
    loadListBox();

    _focus = _controls.front();
}

void ModulesGui::loadLabel() {
    Control::Extent extent(_screenCenter.x - 0.5f * kWidth, kMargin, kWidth, _font->height());

    Control::Text text;
    text.text = "Choose a module to load";
    text.font = _font;

    std::unique_ptr<Label> label(new Label("caption"));
    label->setExtent(extent);
    label->setText(text);

    _controls.push_back(std::move(label));
}

void ModulesGui::loadListBox() {
    int y = _font->height() + 2 * kMargin;
    int h = _opts.height - _font->height() -  3 * kMargin;
    Control::Extent extent(_screenCenter.x - 0.5f * kWidth, y, kWidth, h);

    Control::Text text;
    text.font = _font;

    std::unique_ptr<ListBox> listBox(new ListBox("modules"));
    listBox->setExtent(extent);
    listBox->setText(text);
    listBox->loadCustom();
    listBox->setOnItemClicked(std::bind(&ModulesGui::onItemClicked, this, _1, _2));

    ResourceManager &resources = ResourceManager::instance();
    for (auto &name : resources.moduleNames()) {
        ListBox::Item item;
        item.tag = name;
        item.text = name;
        listBox->add(std::move(item));
    }

    _controls.push_back(std::move(listBox));
}

void ModulesGui::onItemClicked(const std::string &control, const std::string &item) {
    if (_onModuleSelected) {
        _onModuleSelected(item);
    }
}

void ModulesGui::setOnModuleSelected(const std::function<void(const std::string &)> &fn) {
    _onModuleSelected = fn;
}

} // namespace gui

} // namespace reone
