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

#include "dialog.h"

#include "../../gui/control/listbox.h"
#include "../../gui/control/panel.h"
#include "../../resources/manager.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

static glm::vec3 g_kotorBaseColor { 0.0f, 0.639216f, 0.952941f };
static glm::vec3 g_kotorHilightColor { 0.980392f, 1.0f, 0.0f };
static glm::vec3 g_tslBaseColor { 0.192157f, 0.768627f, 0.647059f };
static glm::vec3 g_tslHilightColor { 0.768627f, 0.768627f, 0.686275f };

DialogGui::DialogGui(const render::GraphicsOptions &opts) : GUI(opts) {
    _scaling = ScalingMode::Resize;
}

static string getResRef(GameVersion version) {
    string resRef("dialog");
    if (version == GameVersion::TheSithLords) {
        resRef += "_p";
    }
    return move(resRef);
}

void DialogGui::load(GameVersion version) {
    GUI::load(getResRef(version), BackgroundType::None);
    _version = version;

    configureMessage();
    configureReplies();
    addTopFrame();
    addBottomFrame();
}

void DialogGui::addTopFrame() {
    addFrame(-_rootControl->extent().top, getControl("LBL_MESSAGE").extent().height);
}

void DialogGui::addBottomFrame() {
    int rootTop = _rootControl->extent().top;
    int height = _opts.height - rootTop;

    addFrame(_opts.height - rootTop - height, height);
}

void DialogGui::addFrame(int top, int height) {
    shared_ptr<Panel> frame(new Panel());

    Control::Extent extent;
    extent.left = -_rootControl->extent().left;
    extent.top = top;
    extent.width = _opts.width;
    extent.height = height;

    Control::Border border;
    border.fill = ResMan.findTexture("blackfill", TextureType::Diffuse);

    frame->setExtent(move(extent));
    frame->setBorder(move(border));

    _controls.insert(_controls.begin(), move(frame));
}

void DialogGui::configureMessage() {
    Control &message = getControl("LBL_MESSAGE");

    Control::Extent extent(message.extent());
    extent.top = -_rootControl->extent().top;

    Control::Text text(message.text());
    text.text = "Hello, world!";
    text.color = _version == GameVersion::KotOR ? g_kotorBaseColor : g_tslBaseColor;

    message.setExtent(move(extent));
    message.setText(move(text));
}

void DialogGui::configureReplies() {
    ListBox &replies = static_cast<ListBox &>(getControl("LB_REPLIES"));
    Control &protoItem = replies.protoItem();

    Control::Border hilight;
    hilight.color = _version == GameVersion::KotOR ? g_kotorHilightColor : g_tslHilightColor;

    Control::Text text(protoItem.text());
    text.color = _version == GameVersion::KotOR ? g_kotorBaseColor : g_tslBaseColor;

    protoItem.setHilight(move(hilight));
    protoItem.setText(move(text));

    replies.add(ListBox::Item { "", "1. Reply 1" });
    replies.add(ListBox::Item { "", "2. Reply 2" });
    replies.add(ListBox::Item { "", "3. Reply 3" });
    replies.add(ListBox::Item { "", "4. Reply 4" });
    replies.add(ListBox::Item { "", "5. Reply 5" });
    replies.add(ListBox::Item { "", "6. Reply 6" });
    replies.add(ListBox::Item { "", "7. Reply 7" });
    replies.add(ListBox::Item { "", "8. Reply 8" });
    replies.add(ListBox::Item { "", "9. Reply 9" });
    replies.add(ListBox::Item { "", "10. Reply 10" });
}

} // namespace game

} // namespace reone
