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

#include <boost/format.hpp>

#include "../../audio/player.h"
#include "../../gui/control/listbox.h"
#include "../../gui/control/panel.h"
#include "../../resources/manager.h"
#include "../../script/execution.h"
#include "../../script/manager.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;
using namespace reone::script;

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

    replies.setOnItemClicked([this](const string &ctrl, const string &item) {
        int replyIdx = stoi(item);
        onReplyClicked(replyIdx);
    });
}

void DialogGui::onReplyClicked(int index) {
    const Dialog::EntryReply &reply = _dialog->getReply(index);
    if (reply.entries.empty()) {
        if (_onDialogFinished) _onDialogFinished();
        return;
    }
    int entryIdx = -1;

    for (auto &link : reply.entries) {
        if (link.active.empty()) {
            entryIdx = link.index;
            continue;
        }
        if (checkCondition(link.active)) {
            entryIdx = link.index;
            break;
        }
    }

    if (entryIdx != -1) {
        _currentEntry.reset(new Dialog::EntryReply(_dialog->getEntry(entryIdx)));
        loadCurrentEntry();
    }
}

void DialogGui::startDialog(const string &resRef) {
    shared_ptr<GffStruct> dlg(ResMan.findGFF(resRef, ResourceType::Conversation));
    if (!dlg) {
        if (_onDialogFinished) _onDialogFinished();
        return;
    }

    _dialog.reset(new Dialog());
    _dialog->load(resRef, *dlg);

    loadStartEntry();
}

void DialogGui::loadStartEntry() {
    int entryIdx = -1;
    for (auto &link : _dialog->startEntries()) {
        if (link.active.empty()) {
            entryIdx = link.index;
            continue;
        }
        if (checkCondition(link.active)) {
            entryIdx = link.index;
            break;
        }
    }
    if (entryIdx != -1) {
        _currentEntry.reset(new Dialog::EntryReply(_dialog->getEntry(entryIdx)));
        loadCurrentEntry();
    }
}

bool DialogGui::checkCondition(const string &script) {
    shared_ptr<ScriptProgram> program(ScriptMan.find(script));
    return ScriptExecution(program, ExecutionContext()).run() != 0;
}

void DialogGui::loadCurrentEntry() {
    if (_currentVoice) _currentVoice->stop();

    assert(_currentEntry);
    if (!_currentEntry->voResRef.empty()) {
        shared_ptr<AudioStream> voice(ResMan.findAudio(_currentEntry->voResRef));
        if (voice) {
            _currentVoice = TheAudioPlayer.play(voice);
        }
    }

    Control &message = getControl("LBL_MESSAGE");
    message.setTextMessage(_currentEntry->text);

    ListBox &replies = static_cast<ListBox &>(getControl("LB_REPLIES"));
    replies.clearItems();

    int replyCount = 0;
    for (auto &link : _currentEntry->replies) {
        if (!link.active.empty() && !checkCondition(link.active)) continue;

        string text(_dialog->getReply(link.index).text);
        if (text.empty()) text = "[continue]";

        replies.add({ to_string(link.index), str(boost::format("%d. %s") % ++replyCount % text) });
    }

    if (!_currentEntry->script.empty()) {
        ScriptExecution(ScriptMan.find(_currentEntry->script), ExecutionContext()).run();
    }
    if (replyCount == 0 && _onDialogFinished) {
        _onDialogFinished();
    }
}

void DialogGui::setOnDialogFinished(const std::function<void()> &fn) {
    _onDialogFinished = fn;
}

} // namespace game

} // namespace reone
