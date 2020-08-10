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
#include "../../resources/resources.h"
#include "../../script/execution.h"

#include "../script/routines.h"
#include "../script/util.h"
#include "../types.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;
using namespace reone::script;

namespace reone {

namespace game {

DialogGui::DialogGui(const render::GraphicsOptions &opts) : GUI(opts) {
    _scaling = ScalingMode::Stretch;
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
    int height = _gfxOpts.height - rootTop;

    addFrame(_gfxOpts.height - rootTop - height, height);
}

void DialogGui::addFrame(int top, int height) {
    shared_ptr<Panel> frame(new Panel());

    Control::Extent extent;
    extent.left = -_rootControl->extent().left;
    extent.top = top;
    extent.width = _gfxOpts.width;
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
    text.color = _version == GameVersion::KotOR ? getKotorBaseColor() : getTslBaseColor();

    message.setExtent(move(extent));
    message.setText(move(text));
}

void DialogGui::configureReplies() {
    ListBox &replies = static_cast<ListBox &>(getControl("LB_REPLIES"));
    Control &protoItem = replies.protoItem();

    Control::Border hilight;
    hilight.color = _version == GameVersion::KotOR ? getKotorHilightColor() : getTslHilightColor();

    Control::Text text(protoItem.text());
    text.color = _version == GameVersion::KotOR ? getKotorBaseColor() : getTslBaseColor();

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
        if (_onDialogFinished) {
            if (_onSpeakerChanged) {
                _onSpeakerChanged(_currentSpeaker, "");
            }
            _onDialogFinished();
        }
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

void DialogGui::startDialog(const Object &owner, const string &resRef) {
    shared_ptr<GffStruct> dlg(ResMan.findGFF(resRef, ResourceType::Conversation));
    if (!dlg) {
        if (_onDialogFinished) _onDialogFinished();
        return;
    }
    _dialog.reset(new Dialog());
    _dialog->load(resRef, *dlg);

    _currentSpeaker = owner.tag();
    if (_onSpeakerChanged) {
        _onSpeakerChanged("", _currentSpeaker);
    }

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
    if (entryIdx == -1) {
        if (_onDialogFinished) _onDialogFinished();
        return;
    }
    _currentEntry.reset(new Dialog::EntryReply(_dialog->getEntry(entryIdx)));
    loadCurrentEntry();
}

bool DialogGui::checkCondition(const string &script) {
    return runScript(script, kObjectInvalid, kObjectInvalid) != 0;
}

void DialogGui::loadCurrentEntry() {
    if (_currentVoice) _currentVoice->stop();

    assert(_currentEntry);

    if (!_currentEntry->speaker.empty() && _currentSpeaker != _currentEntry->speaker) {
        string prevSpeaker(_currentSpeaker);
        _currentSpeaker = _currentEntry->speaker;
        if (_onSpeakerChanged) {
            _onSpeakerChanged(prevSpeaker, _currentSpeaker);
        }
    }

    shared_ptr<AudioStream> voice;
    if (!_currentEntry->voResRef.empty()) {
        voice = ResMan.findAudio(_currentEntry->voResRef);
    }
    if (!voice && !_currentEntry->sound.empty()) {
        voice = ResMan.findAudio(_currentEntry->sound);
    }
    if (voice) {
        _currentVoice = TheAudioPlayer.play(voice, AudioType::Sound);
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
        runScript(_currentEntry->script, kObjectInvalid, kObjectInvalid);
    }
    if (replyCount == 0 && _onDialogFinished) {
        if (_onSpeakerChanged) {
            _onSpeakerChanged(_currentSpeaker, "");
        }
        _onDialogFinished();
    }
}

void DialogGui::setOnSpeakerChanged(const function<void(const string&, const string &)> &fn) {
    _onSpeakerChanged = fn;
}

void DialogGui::setOnDialogFinished(const function<void()> &fn) {
    _onDialogFinished = fn;
}

} // namespace game

} // namespace reone
