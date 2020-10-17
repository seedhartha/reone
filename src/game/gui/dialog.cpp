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

#include "SDL2/SDL_timer.h"

#include "../../audio/player.h"
#include "../../core/random.h"
#include "../../gui/control/listbox.h"
#include "../../gui/control/panel.h"
#include "../../resource/resources.h"
#include "../../script/execution.h"

#include "../game.h"
#include "../script/routines.h"
#include "../script/util.h"

#include "colors.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::script;

namespace reone {

namespace game {

static const int kDefaultEntryDuration = 1000;

enum EndEntryFlags {
    kEndEntryOnAnimFinish = 1,
    kEndEntryOnAudioStop = 2
};

DialogGui::DialogGui(GameVersion version, Game *game, const GraphicsOptions &opts) :
    GUI(version, opts), _game(game) {

    _resRef = getResRef("dialog");
    _scaling = ScalingMode::Stretch;
}

void DialogGui::load() {
    GUI::load();

    configureMessage();
    configureReplies();
    loadTopFrame();
    loadBottomFrame();
}

void DialogGui::loadTopFrame() {
    addFrame(-_rootControl->extent().top, getControl("LBL_MESSAGE").extent().height);
}

void DialogGui::addFrame(int top, int height) {
    unique_ptr<Panel> frame(new Panel());

    Control::Extent extent;
    extent.left = -_rootControl->extent().left;
    extent.top = top;
    extent.width = _gfxOpts.width;
    extent.height = height;

    frame->setExtent(move(extent));
    frame->setBorderFill("blackfill");

    _controls.insert(_controls.begin(), move(frame));
}

void DialogGui::loadBottomFrame() {
    int rootTop = _rootControl->extent().top;
    int height = _gfxOpts.height - rootTop;

    addFrame(_gfxOpts.height - rootTop - height, height);
}

void DialogGui::configureMessage() {
    Control &message = getControl("LBL_MESSAGE");

    Control::Extent extent(message.extent());
    extent.top = -_rootControl->extent().top;

    message.setExtent(move(extent));
    message.setTextColor(getBaseColor(_version));
}

void DialogGui::configureReplies() {
    ListBox &replies = static_cast<ListBox &>(getControl("LB_REPLIES"));

    Control &protoItem = replies.protoItem();
    protoItem.setHilightColor(getHilightColor(_version));
    protoItem.setTextColor(getBaseColor(_version));

    replies.setOnItemClicked([this](const string &ctrl, const string &item) {
        int replyIdx = stoi(item);
        onReplyClicked(replyIdx);
    });
}

void DialogGui::onReplyClicked(int index) {
    if (!_pickReplyEnabled) return;

    if (_onReplyPicked) {
        _onReplyPicked(index);
    }
    pickReply(index);
}

void DialogGui::startDialog(SpatialObject &owner, const string &resRef) {
    shared_ptr<GffStruct> dlg(Resources.findGFF(resRef, ResourceType::Conversation));
    if (!dlg) {
        if (_onDialogFinished) _onDialogFinished();
        return;
    }
    _owner = &owner;
    _currentSpeaker = _owner;

    _dialog.reset(new DlgFile());
    _dialog->load(resRef, *dlg);

    loadAnimatedCamera();
    loadStartEntry();
}

void DialogGui::loadAnimatedCamera() {
    string modelResRef(_dialog->cameraModel());
    if (modelResRef.empty()) return;

    AnimatedCamera &camera = _game->module()->area()->animatedCamera();
    camera.setModel(modelResRef);
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
    _currentEntry.reset(new DlgFile::EntryReply(_dialog->getEntry(entryIdx)));
    loadCurrentEntry();
}

bool DialogGui::checkCondition(const string &script) {
    int result = runScript(script, _owner->id(), kObjectInvalid, -1);
    return result == -1 || result == 1;
}

void DialogGui::loadCurrentEntry() {
    if (!_currentEntry->script.empty()) {
        runScript(_currentEntry->script, _owner->id(), kObjectInvalid, -1);
    }
    Control &message = getControl("LBL_MESSAGE");
    message.setTextMessage(_currentEntry->text);

    loadReplies();
    loadCurrentSpeaker();
    playVoiceOver();
    scheduleEndOfEntry();
    updateCamera();
}

void DialogGui::loadReplies() {
    ListBox &replies = static_cast<ListBox &>(getControl("LB_REPLIES"));
    replies.clear();

    static vector<int> activeReplies;

    activeReplies.clear();
    for (auto &link : _currentEntry->replies) {
        if (link.active.empty() || checkCondition(link.active)) {
            activeReplies.push_back(link.index);
        }
    }

    bool singleEmptyReply = false;
    int replyNumber = 0;
    for (auto &replyIdx : activeReplies) {
        const DlgFile::EntryReply &reply = _dialog->getReply(replyIdx);
        string text(reply.text);
        if (text.empty()) {
            if (activeReplies.size() == 1) {
                singleEmptyReply = true;
                break;
            } else {
            text = "[empty]";
            }
        }
        replies.add({ to_string(replyIdx), str(boost::format("%d. %s") % ++replyNumber % text) });
    }
    if (singleEmptyReply) {
        _autoPickReplyIdx = activeReplies.front();
    }

    if (activeReplies.empty()) {
        finish();
    } else {
        hideControl("LB_REPLIES");
    }
}

void DialogGui::finish() {
    if (!_dialog->endScript().empty()) {
        runScript(_dialog->endScript(), _owner->id(), kObjectInvalid, -1);
    }
    if (_currentSpeaker) {
        static_cast<Creature &>(*_currentSpeaker).setTalking(false);
    }
    if (_onDialogFinished) {
        _onDialogFinished();
    }
}

void DialogGui::loadCurrentSpeaker() {
    shared_ptr<Area> area(_game->module()->area());
    SpatialObject *speaker = nullptr;

    if (!_currentEntry->speaker.empty()) {
        shared_ptr<SpatialObject> speakerPtr(area->find(_currentEntry->speaker));
        speaker = speakerPtr.get();
    }
    if (!speaker) {
        speaker = _owner;
    }
    if (_currentSpeaker && _currentSpeaker != speaker) {
        Creature &prevSpeakerCreature = static_cast<Creature &>(*_currentSpeaker);
        prevSpeakerCreature.setTalking(false);
    }
    _currentSpeaker = speaker;

    Creature &speakerCreature = static_cast<Creature &>(*_currentSpeaker);
    speakerCreature.setTalking(true);

    shared_ptr<SpatialObject> partyLeader(area->partyLeader());
    partyLeader->face(speakerCreature);

    speakerCreature.face(*partyLeader);
}

void DialogGui::updateCamera() {
    shared_ptr<Area> area(_game->module()->area());
    shared_ptr<SpatialObject> partyLeader(area->partyLeader());
    glm::vec3 listenerPosition(partyLeader->position());
    glm::vec3 speakerPosition(_currentSpeaker->position());
    glm::vec3 hookPosition(0.0f);

    if (partyLeader->model()->getNodeAbsolutePosition("headhook", hookPosition)) {
        listenerPosition += hookPosition;
    }
    if (_currentSpeaker->model()->getNodeAbsolutePosition("headhook", hookPosition)) {
        speakerPosition += hookPosition;
    }
    if (_dialog->cameraModel().empty()) {
        DialogCamera &camera = area->dialogCamera();
        camera.setListenerPosition(listenerPosition);
        camera.setSpeakerPosition(speakerPosition);
        camera.setVariant(getRandomCameraVariant());
    } else {
        AnimatedCamera &camera = area->animatedCamera();
        camera.setFieldOfView(_currentEntry->camFieldOfView != 0.0f ? _currentEntry->camFieldOfView : kDefaultAnimCamFOV);
        camera.playAnimation(_currentEntry->cameraAnimation);
    }
}

DialogCamera::Variant DialogGui::getRandomCameraVariant() const {
    int r = random(0, 2);
    switch (r) {
        case 0:
            return _entryEnded ? DialogCamera::Variant::ListenerClose : DialogCamera::Variant::SpeakerClose;
        case 1:
            return _entryEnded ? DialogCamera::Variant::ListenerFar : DialogCamera::Variant::SpeakerFar;
        default:
            return DialogCamera::Variant::Both;
    }
}

void DialogGui::playVoiceOver() {
    if (_currentVoice) {
        _currentVoice->stop();
        _currentVoice.reset();
    }
    shared_ptr<AudioStream> voice;

    if (!_currentEntry->sound.empty()) {
        voice = Resources.findAudio(_currentEntry->sound);
    }
    if (!voice && !_currentEntry->voResRef.empty()) {
        voice = Resources.findAudio(_currentEntry->voResRef);
    }
    if (voice) {
        _currentVoice = TheAudioPlayer.play(voice, AudioType::Sound);
    }
}

void DialogGui::scheduleEndOfEntry() {
    _entryEnded = false;
    _endEntryFlags = 0;

    uint32_t now = SDL_GetTicks();

    if (!_dialog->cameraModel().empty() && _currentEntry->waitFlags & kDialogWaitAnimFinish) {
        _endEntryFlags = kEndEntryOnAnimFinish;
        return;
    }
    if (_currentEntry->delay != -1) {
        _endEntryTimestamp = now + 1000 * _currentEntry->delay;
        return;
    }
    if (_currentVoice) {
        _endEntryFlags = kEndEntryOnAudioStop;
        _endEntryTimestamp = now + _currentVoice->duration();
        return;
    }
    _endEntryTimestamp = now + kDefaultEntryDuration;
}

void DialogGui::pickReply(uint32_t index) {
    const DlgFile::EntryReply &reply = _dialog->getReply(index);

    if (!reply.script.empty()) {
        runScript(reply.script, _owner->id(), kObjectInvalid, -1);
    }
    if (reply.entries.empty()) {
        finish();
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
        _currentEntry.reset(new DlgFile::EntryReply(_dialog->getEntry(entryIdx)));
        loadCurrentEntry();
    }
}

bool DialogGui::handle(const SDL_Event &event) {
    if (!_entryEnded &&
        _dialog->isSkippable() &&
        event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT) {

        endCurrentEntry();
        return true;
    }

    return GUI::handle(event);
}

void DialogGui::endCurrentEntry() {
    _entryEnded =  true;

    if (_currentVoice) {
        _currentVoice->stop();
    }
    if (_autoPickReplyIdx != -1) {
        int replyIdx = _autoPickReplyIdx;
        _autoPickReplyIdx = -1;
        pickReply(replyIdx);
    } else {
        showControl("LB_REPLIES");
        updateCamera();
    }
}

bool DialogGui::handleKeyDown(SDL_Scancode key) {
    return false;
}

bool DialogGui::handleKeyUp(SDL_Scancode key) {
    if (!_entryEnded) return false;

    if (key >= SDL_SCANCODE_1 && key <= SDL_SCANCODE_9) {
        ListBox &replies = static_cast<ListBox &>(getControl("LB_REPLIES"));
        int itemIdx = key - SDL_SCANCODE_1;
        if (itemIdx < replies.itemCount()) {
            const ListBox::Item &item = replies.getItemAt(itemIdx);
            onReplyClicked(stoi(item.tag));
            return true;
        }
    }

    return false;
}

void DialogGui::update(float dt) {
    GUI::update(dt);

    if (!_entryEnded) {
        bool endOnAnimFinish = (_endEntryFlags & kEndEntryOnAnimFinish) != 0;
        bool endOnAudioStop = (_endEntryFlags & kEndEntryOnAudioStop) != 0;

        if (endOnAnimFinish) {
            shared_ptr<Area> area(_game->module()->area());
            AnimatedCamera &camera = area->animatedCamera();
            camera.update(dt);
            if (camera.isAnimationFinished()) {
                endCurrentEntry();
            }
        } else if (endOnAudioStop) {
            bool stopped = _currentVoice && _currentVoice->stopped();
            if (stopped) {
                endCurrentEntry();
            }
        } else {
            uint32_t now = SDL_GetTicks();
            if (now >= _endEntryTimestamp) {
                endCurrentEntry();
            }
        }
    }
}

Camera &DialogGui::camera() const {
    string cameraModel(_dialog->cameraModel());
    shared_ptr<Area> area(_game->module()->area());

    return cameraModel.empty() ? area->dialogCamera() : static_cast<Camera &>(area->animatedCamera());
}

void DialogGui::setPickReplyEnabled(bool enabled) {
    _pickReplyEnabled = enabled;
}

void DialogGui::setOnReplyPicked(const function<void(uint32_t)> &fn) {
    _onReplyPicked = fn;
}

void DialogGui::setOnDialogFinished(const function<void()> &fn) {
    _onDialogFinished = fn;
}

} // namespace game

} // namespace reone
