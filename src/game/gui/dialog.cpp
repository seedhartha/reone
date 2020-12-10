/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../audio/files.h"
#include "../../audio/player.h"
#include "../../audio/soundhandle.h"
#include "../../common/log.h"
#include "../../common/random.h"
#include "../../gui/control/listbox.h"
#include "../../gui/control/panel.h"
#include "../../resource/resources.h"
#include "../../script/execution.h"

#include "../game.h"
#include "../script/routines.h"
#include "../script/util.h"

#include "colorutil.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static const int kDefaultEntryDuration = 1000;

enum EndEntryFlags {
    kEndEntryOnAnimFinish = 1,
    kEndEntryOnAudioStop = 2
};

DialogGUI::DialogGUI(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("dialog");
    _scaling = ScalingMode::Stretch;
}

void DialogGUI::load() {
    GUI::load();

    configureMessage();
    configureReplies();
    loadTopFrame();
    loadBottomFrame();
}

void DialogGUI::loadTopFrame() {
    addFrame(-_rootControl->extent().top, getControl("LBL_MESSAGE").extent().height);
}

void DialogGUI::addFrame(int top, int height) {
    unique_ptr<Panel> frame(new Panel(this));

    Control::Extent extent;
    extent.left = -_rootControl->extent().left;
    extent.top = top;
    extent.width = _gfxOpts.width;
    extent.height = height;

    frame->setExtent(move(extent));
    frame->setBorderFill("blackfill");

    _controls.insert(_controls.begin(), move(frame));
}

void DialogGUI::loadBottomFrame() {
    int rootTop = _rootControl->extent().top;
    int height = _gfxOpts.height - rootTop;

    addFrame(_gfxOpts.height - rootTop - height, height);
}

void DialogGUI::configureMessage() {
    Control &message = getControl("LBL_MESSAGE");

    Control::Extent extent(message.extent());
    extent.top = -_rootControl->extent().top;

    message.setExtent(move(extent));
    message.setTextColor(getBaseColor(_version));
}

void DialogGUI::configureReplies() {
    ListBox &replies = static_cast<ListBox &>(getControl("LB_REPLIES"));

    Control &protoItem = replies.protoItem();
    protoItem.setHilightColor(getHilightColor(_version));
    protoItem.setTextColor(getBaseColor(_version));
}

void DialogGUI::onReplyClicked(int index) {
    pickReply(index);
}

void DialogGUI::onListBoxItemClick(const string &control, const string &item) {
    if (control != "LB_REPLIES") return;

    int replyIdx = stoi(item);
    onReplyClicked(replyIdx);
}

void DialogGUI::startDialog(SpatialObject &owner, const string &resRef) {
    shared_ptr<GffStruct> dlg(Resources::instance().getGFF(resRef, ResourceType::Conversation));
    if (!dlg) {
        _game->openInGame();
        return;
    }
    _owner = &owner;
    _currentSpeaker = _owner;

    _dialog.reset(new Dialog());
    _dialog->load(resRef, *dlg);

    loadAnimatedCamera();
    loadStartEntry();
}

void DialogGUI::loadAnimatedCamera() {
    string modelResRef(_dialog->cameraModel());
    if (modelResRef.empty()) return;

    AnimatedCamera &camera = static_cast<AnimatedCamera &>(_game->module()->area()->getCamera(CameraType::Animated));
    camera.setModel(modelResRef);
}

void DialogGUI::loadStartEntry() {
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
        _game->openInGame();
        return;
    }
    _currentEntry.reset(new Dialog::EntryReply(_dialog->getEntry(entryIdx)));
    loadCurrentEntry();
}

bool DialogGUI::checkCondition(const string &script) {
    int result = runScript(script, _owner->id(), kObjectInvalid, -1);
    return result == -1 || result == 1;
}

void DialogGUI::loadCurrentEntry() {
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

void DialogGUI::loadReplies() {
    ListBox &replies = static_cast<ListBox &>(getControl("LB_REPLIES"));
    replies.clear();

    vector<int> activeReplies;
    for (auto &link : _currentEntry->replies) {
        if (link.active.empty() || checkCondition(link.active)) {
            activeReplies.push_back(link.index);
        }
    }

    bool singleEmptyReply = false;
    int replyNumber = 0;
    for (auto &replyIdx : activeReplies) {
        const Dialog::EntryReply &reply = _dialog->getReply(replyIdx);
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

void DialogGUI::finish() {
    if (!_dialog->endScript().empty()) {
        runScript(_dialog->endScript(), _owner->id(), kObjectInvalid, -1);
    }
    if (_currentSpeaker) {
        Creature *speakerCreature = dynamic_cast<Creature *>(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->setTalking(false);
        }
    }
    _game->openInGame();
}

void DialogGUI::loadCurrentSpeaker() {
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
        Creature *prevSpeakerCreature = dynamic_cast<Creature *>(_currentSpeaker);
        if (prevSpeakerCreature) {
            prevSpeakerCreature->setTalking(false);
        }
    }
    _currentSpeaker = speaker;

    shared_ptr<Creature> player(_game->party().player());

    if (_currentSpeaker) {
        player->face(*_currentSpeaker);

        Creature *speakerCreature = dynamic_cast<Creature *>(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->setTalking(true);
            speakerCreature->face(*player);
        }
    }
}

void DialogGUI::updateCamera() {
    shared_ptr<Area> area(_game->module()->area());

    if (_dialog->cameraModel().empty()) {
        shared_ptr<Creature> player(_game->party().player());
        glm::vec3 listenerPosition(player ? getTalkPosition(*player) : glm::vec3(0.0f));
        glm::vec3 speakerPosition(_currentSpeaker ? getTalkPosition(*_currentSpeaker) : glm::vec3(0.0f));
        DialogCamera &camera = static_cast<DialogCamera &>(area->getCamera(CameraType::Dialog));
        camera.setListenerPosition(listenerPosition);
        camera.setSpeakerPosition(speakerPosition);
        camera.setVariant(getRandomCameraVariant());
    } else {
        AnimatedCamera &camera = static_cast<AnimatedCamera &>(area->getCamera(CameraType::Animated));
        camera.setFieldOfView(_currentEntry->camFieldOfView != 0.0f ? _currentEntry->camFieldOfView : kDefaultAnimCamFOV);
        camera.playAnimation(_currentEntry->cameraAnimation);
    }
}

glm::vec3 DialogGUI::getTalkPosition(const SpatialObject &object) const {
    shared_ptr<ModelSceneNode> model(object.model());
    if (model) {
        glm::vec3 hookPosition(0.0f);
        if (model->getNodeAbsolutePosition("talkdummy", hookPosition)) {
            return object.position() + hookPosition;
        }
        return model->getCenterOfAABB();
    }

    return object.position();
}

DialogCamera::Variant DialogGUI::getRandomCameraVariant() const {
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

void DialogGUI::playVoiceOver() {
    if (_currentVoice) {
        _currentVoice->stop();
        _currentVoice.reset();
    }
    shared_ptr<AudioStream> voice;

    if (!_currentEntry->sound.empty()) {
        voice = AudioFiles::instance().get(_currentEntry->sound);
    }
    if (!voice && !_currentEntry->voResRef.empty()) {
        voice = AudioFiles::instance().get(_currentEntry->voResRef);
    }
    if (voice) {
        _currentVoice = AudioPlayer::instance().play(voice, AudioType::Sound);
    }
}

void DialogGUI::scheduleEndOfEntry() {
    _entryEnded = false;
    _endEntryFlags = 0;

    if (!_dialog->cameraModel().empty() && _currentEntry->waitFlags & kDialogWaitAnimFinish) {
        _endEntryFlags = kEndEntryOnAnimFinish;
        return;
    }
    if (_currentEntry->delay != -1) {
        _endEntryTimeout = 1000.0f * _currentEntry->delay;
        return;
    }
    if (_currentVoice) {
        _endEntryFlags = kEndEntryOnAudioStop;
        _endEntryTimeout = static_cast<float>(_currentVoice->duration());
        return;
    }
    _endEntryTimeout = kDefaultEntryDuration;
}

void DialogGUI::pickReply(uint32_t index) {
    const Dialog::EntryReply &reply = _dialog->getReply(index);

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

    if (entryIdx == -1) {
        finish();
        return;
    }
    _currentEntry.reset(new Dialog::EntryReply(_dialog->getEntry(entryIdx)));
    loadCurrentEntry();
}

bool DialogGUI::handle(const SDL_Event &event) {
    if (!_entryEnded &&
        _dialog->isSkippable() &&
        event.type == SDL_MOUSEBUTTONDOWN) {

        endCurrentEntry();
        return true;
    }

    return GUI::handle(event);
}

void DialogGUI::endCurrentEntry() {
    _entryEnded = true;

    if (_currentVoice) {
        _currentVoice->stop();
        _currentVoice.reset();
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

bool DialogGUI::handleKeyDown(SDL_Scancode key) {
    return false;
}

bool DialogGUI::handleKeyUp(SDL_Scancode key) {
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

void DialogGUI::update(float dt) {
    GUI::update(dt);

    if (_currentSpeaker && _game->cameraType() == CameraType::Dialog) {
        DialogCamera &camera = static_cast<DialogCamera &>(_game->module()->area()->getCamera(CameraType::Dialog));
        camera.setSpeakerPosition(getTalkPosition(*_currentSpeaker));
    }
    if (!_entryEnded) {
        bool endOnAnimFinish = (_endEntryFlags & kEndEntryOnAnimFinish) != 0;
        if (endOnAnimFinish) {
            shared_ptr<Area> area(_game->module()->area());
            AnimatedCamera &camera = static_cast<AnimatedCamera &>(area->getCamera(CameraType::Animated));
            if (camera.isAnimationFinished()) {
                endCurrentEntry();
            }
        } else {
            _endEntryTimeout = glm::max(0.0f, _endEntryTimeout - dt);
            if (_endEntryTimeout == 0.0f) {
                endCurrentEntry();
            } else {
                bool endOnAudioStop = (_endEntryFlags & kEndEntryOnAudioStop) != 0;
                bool stopped = _currentVoice && _currentVoice->isStopped();
                if (endOnAudioStop && stopped) {
                    endCurrentEntry();
                }
            }
        }
    }
}

CameraType DialogGUI::getCamera(int &cameraId) const {
    string cameraModel(_dialog->cameraModel());
    if (!cameraModel.empty()) {
        return CameraType::Animated;
    }
    if (_currentEntry->cameraId != -1) {
        cameraId = _currentEntry->cameraId;
        return CameraType::Static;
    }

    return CameraType::Dialog;
}

} // namespace game

} // namespace reone
